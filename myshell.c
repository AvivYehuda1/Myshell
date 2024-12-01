#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

// Required function declarations
int prepare(void);
int process_arglist(int count, char **arglist);
int finalize(void);

// SIGCHLD handler to clean up zombies
void handle_sigchld(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0) {}
}

//Initial setup for the shell
int prepare(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    if (sigaction(SIGCHLD, &sa, NULL)== -1) {
        perror("Error: Unable to set SIGCHLD handler");
        return -1;
    }
    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGINT, &sa, NULL)== -1) {
        perror("Error: Unable to set SIGINT handler");
        return -1;
    }

    return 0;
}

// Cleanup before shell exit
int finalize(void) {
    return 0;
}

// Processes user commands
int process_arglist(int count, char **arglist) {
    int background = 0;
    int inputFileIndex = -1;
    int outputFileIndex = -1;
    int pipeIndex = -1;

    //Check if the command should run in the background
    if (strcmp(arglist[count-1], "&")== 0) {
        background= 1;
        arglist[count-1] = NULL;
        count--;
    }

    // Check for input redirection, output redirection, and pipe
    for (int i = 0; i< count; i++) {
        if (strcmp(arglist[i], "<")== 0) {
            inputFileIndex = i;
        } else if (strcmp(arglist[i], ">>")== 0) {
            outputFileIndex = i;
        } else if (strcmp(arglist[i], "|")== 0) {
            pipeIndex = i;
        }
    }

    //Handling pipes
    if (pipeIndex != -1) {
        arglist[pipeIndex] = NULL; 
        int pipe_fd[2];
        if (pipe(pipe_fd)== -1) {
            perror("Error: Pipe failed");
            return 0;
        }
        int pid1 = fork();
        if (pid1 == 0) {
            //First child process: execute the command before the pipe
            close(pipe_fd[0]); 
            dup2(pipe_fd[1], STDOUT_FILENO);  
            close(pipe_fd[1]);
            execvp(arglist[0], arglist);
            perror("Error: Execvp failed");
            exit(1);
        }
        int pid2 = fork();
        if (pid2 == 0) {
            // Second child process: execute the command after the pipe
            close(pipe_fd[1]); 
            dup2(pipe_fd[0], STDIN_FILENO); 
            close(pipe_fd[0]);
            execvp(arglist[pipeIndex+1], &arglist[pipeIndex+1]);
            perror("Error: Execvp failed");
            exit(1);
        }
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        if (!background) {
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        }
        return 1;
    }

    //Execute command
    if (count > 0) {
        int pid = fork();
        if (pid == 0) {
            // Child process should handle redirections
            if (inputFileIndex != -1) {
                int fd = open(arglist[inputFileIndex+1], O_RDONLY);
                if (fd == -1) {
                    perror("Error: Unable to open the specified input file");
                    exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
                arglist[inputFileIndex]= NULL;
            }
            if (outputFileIndex!= -1) {
                int fd = open(arglist[outputFileIndex+1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (fd == -1) {
                    perror("Error: Unable to open file for output redirection");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
                arglist[outputFileIndex]= NULL;
            }

            //Child process should also ignore SIGINT if running in the background
            if (!background) {
                signal(SIGINT, SIG_DFL); 
            } else {
                signal(SIGINT, SIG_IGN);
            }

            // Child process executes the command
            execvp(arglist[0], arglist);
            perror("Error: Execvp failed");
            exit(1);
        } else if (pid > 0) {
            // Parent process
            if (!background) {
                int status;
                waitpid(pid, &status, 0);
            }
        } else {
            perror("Error: Fork failed");
            return 0;
        }
    }
    return 1;
}
