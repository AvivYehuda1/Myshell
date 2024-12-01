# Custom Linux Shell

## Overview
This project is a custom Linux shell implemented in C as part of an Operating Systems course. The shell demonstrates core operating system concepts, including process management, command parsing, I/O redirection, and support for piping.

The project showcases:
- **Process management**: Execution of foreground and background processes.
- **Command parsing**: Handling built-in and external commands.
- **I/O Redirection**: Support for input (`<`), output (`>`), and append (`>>`) redirection.
- **Piping**: Execution of commands connected through pipes (`|`).
- **Signal handling**: Ignoring `SIGINT` in background processes and proper cleanup of zombie processes.
- **Exit functionality**: Clean termination of the shell.

## Features
1. **Execute Commands**: Run standard Linux commands like `ls`, `cat`, and more.
2. **Background Processes**: Support for commands with `&` to run in the background.
3. **I/O Redirection**:
   - Redirect input from files using `<`.
   - Redirect output to files using `>`.
   - Append output to files using `>>`.
4. **Piping**: Support for commands connected with `|`, enabling output from one command to serve as input for another.
5. **Error Handling**: Handles invalid commands, missing files, and other errors gracefully.

## Getting Started

### Prerequisites
- Linux-based operating system
- GCC Compiler

### Building the Shell
To compile the shell:
gcc -o myshell myshell.c

## Running the Shell
To run the shell:
./myshell

## Example Usage
### Run a command
ls

### Run a background process
sleep 10 &

### Redirect output to a file
echo "Hello World" > output.txt

### Append output to a file
echo "Appending this line" >> output.txt

### Redirect input from a file
wc < input.txt

### Pipe commands
ls | grep .c

## Project Structure
- myshell.c: Main implementation of the shell logic.

## Future Enhancements
- Add support for additional built-in commands (e.g., cd).
- Implement more robust error messages and debugging options.
- Expand multi-pipeline support for commands.
