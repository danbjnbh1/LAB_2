#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
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
        
        // Execute the command
        execute(parsedLine);
        
        // Release resources
        freeCmdLines(parsedLine);
    }
    
    return 0;
}

