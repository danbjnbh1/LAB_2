#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include "LineParser.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define INPUT_SIZE 2048

// Global debug flag
int debug_mode = 0;

void execute(cmdLine *pCmdLine) {
    pid_t pid;
    
    // Fork to create child process
    pid = fork();
    
    if (pid == -1) {
        // Fork failed
        perror("fork failed");
        return;
    }
    
    if (pid == 0) {
        // Child process
        if (debug_mode) {
            fprintf(stderr, "PID: %d\n", getpid());
            fprintf(stderr, "Executing command: %s\n", pCmdLine->arguments[0]);
        }
        
        // Handle input redirection (<)
        if (pCmdLine->inputRedirect != NULL) {
            int fd_in = open(pCmdLine->inputRedirect, O_RDONLY);
            if (fd_in < 0) {
                perror("open input file failed");
                _exit(1);
            }
            
            // Redirect stdin (fd 0) to the input file
            if (dup2(fd_in, STDIN_FILENO) < 0) {
                perror("dup2 input failed");
                close(fd_in);
                _exit(1);
            }
            close(fd_in);  // Close original fd after dup2
            
            if (debug_mode) {
                fprintf(stderr, "Input redirected from: %s\n", pCmdLine->inputRedirect);
            }
        }
        
        // Handle output redirection (>)
        if (pCmdLine->outputRedirect != NULL) {
            int fd_out = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out < 0) {
                perror("open output file failed");
                _exit(1);
            }
            
            // Redirect stdout (fd 1) to the output file
            if (dup2(fd_out, STDOUT_FILENO) < 0) {
                perror("dup2 output failed");
                close(fd_out);
                _exit(1);
            }
            close(fd_out);  // Close original fd after dup2
            
            if (debug_mode) {
                fprintf(stderr, "Output redirected to: %s\n", pCmdLine->outputRedirect);
            }
        }
        
        // Execute the command using execvp (searches in PATH)
        execvp(pCmdLine->arguments[0], (char * const *)pCmdLine->arguments);
        
        // If execvp returns, it failed
        perror("execvp failed");
        _exit(1);  // Use _exit in child process
    }
    
    // Parent process continues here
    
    // Wait for child if blocking (foreground)
    if (pCmdLine->blocking) {
        int status;
        waitpid(pid, &status, 0);
        
        if (debug_mode) {
            fprintf(stderr, "Child process %d terminated\n", pid);
        }
    } else {
        // Background process - don't wait
        if (debug_mode) {
            fprintf(stderr, "Running in background: PID %d\n", pid);
        }
    }
}

int main(int argc, char *argv[]) {
    char cwd[PATH_MAX];
    char input[INPUT_SIZE];
    cmdLine *parsedLine;
    
    // Check for debug flag
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        debug_mode = 1;
        fprintf(stderr, "Debug mode enabled\n");
    }
    
    // Infinite loop for the shell
    while (1) {
        // Display prompt - current working directory
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s> ", cwd);
        } else {
            perror("getcwd failed");
            printf("> ");
        }
        
        // Read input from user
        if (fgets(input, INPUT_SIZE, stdin) == NULL) {
            break;  // Exit on EOF (Ctrl+D)
        }
        
        // Parse the input
        parsedLine = parseCmdLines(input);
        
        // Check if parsing was successful
        if (parsedLine == NULL) {
            continue;  // Empty line or parse error
        }
        
        // Check for "quit" command
        if (strcmp(parsedLine->arguments[0], "quit") == 0) {
            freeCmdLines(parsedLine);
            break;  // Exit normally
        }
        
        // Check for "cd" command (must run in shell process, not child)
        if (strcmp(parsedLine->arguments[0], "cd") == 0) {
            if (parsedLine->argCount < 2) {
                fprintf(stderr, "cd: missing argument\n");
            } else {
                if (chdir(parsedLine->arguments[1]) != 0) {
                    perror("cd failed");
                }
            }
            freeCmdLines(parsedLine);
            continue;  // Don't execute, continue to next iteration
        }
        
        // Check for "blast" command - send SIGINT
        if (strcmp(parsedLine->arguments[0], "blast") == 0) {
            if (parsedLine->argCount < 2) {
                fprintf(stderr, "blast: missing PID argument\n");
            } else {
                pid_t target_pid = atoi(parsedLine->arguments[1]);
                if (kill(target_pid, SIGINT) == 0) {
                    if (debug_mode) {
                        fprintf(stderr, "Sent SIGINT to process %d\n", target_pid);
                    }
                } else {
                    perror("blast failed");
                }
            }
            freeCmdLines(parsedLine);
            continue;
        }
        
        // Check for "zzzz" command - send SIGTSTP
        if (strcmp(parsedLine->arguments[0], "zzzz") == 0) {
            if (parsedLine->argCount < 2) {
                fprintf(stderr, "zzzz: missing PID argument\n");
            } else {
                pid_t target_pid = atoi(parsedLine->arguments[1]);
                if (kill(target_pid, SIGTSTP) == 0) {
                    if (debug_mode) {
                        fprintf(stderr, "Sent SIGTSTP to process %d\n", target_pid);
                    }
                } else {
                    perror("zzzz failed");
                }
            }
            freeCmdLines(parsedLine);
            continue;
        }
        
        // Check for "kuku" command - send SIGCONT
        if (strcmp(parsedLine->arguments[0], "kuku") == 0) {
            if (parsedLine->argCount < 2) {
                fprintf(stderr, "kuku: missing PID argument\n");
            } else {
                pid_t target_pid = atoi(parsedLine->arguments[1]);
                if (kill(target_pid, SIGCONT) == 0) {
                    if (debug_mode) {
                        fprintf(stderr, "Sent SIGCONT to process %d\n", target_pid);
                    }
                } else {
                    perror("kuku failed");
                }
            }
            freeCmdLines(parsedLine);
            continue;
        }
        
        // Execute the command
        execute(parsedLine);
        
        // Release resources
        freeCmdLines(parsedLine);
    }
    
    return 0;
}

