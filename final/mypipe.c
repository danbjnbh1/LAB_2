#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int pipefd[2];  // pipe file descriptors: [0] = read, [1] = write
    pid_t pid;
    char buffer[1024];
    
    // Check if message argument is provided
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        exit(1);
    }
    
    // Create the pipe
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(1);
    }
    
    // Fork to create child process
    pid = fork();
    
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }
    
    if (pid == 0) {
        // Child process - writes to pipe
        
        // Close read end (not needed in child)
        close(pipefd[0]);
        
        // Write message to pipe
        write(pipefd[1], argv[1], strlen(argv[1]) + 1);
        
        // Close write end
        close(pipefd[1]);
        
        // Exit child
        exit(0);
    } else {
        // Parent process - reads from pipe
        
        // Close write end (not needed in parent)
        close(pipefd[1]);
        
        // Read message from pipe
        read(pipefd[0], buffer, sizeof(buffer));
        
        // Print the message
        printf("%s\n", buffer);
        
        // Close read end
        close(pipefd[0]);
        
        // Wait for child to finish
        wait(NULL);
    }
    
    return 0;
}


