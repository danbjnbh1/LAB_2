#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "LineParser.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define INPUT_SIZE 2048

void execute(cmdLine *pCmdLine) {
    // Execute the command using execv
    execv(pCmdLine->arguments[0], (char * const *)pCmdLine->arguments);
    
    // If execv returns, it failed
    perror("execv failed");
    exit(1);
}

int main(int argc, char *argv[]) {
    char cwd[PATH_MAX];
    char input[INPUT_SIZE];
    cmdLine *parsedLine;
    
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
        
        // Execute the command
        execute(parsedLine);
        
        // Release resources
        freeCmdLines(parsedLine);
    }
    
    return 0;
}

