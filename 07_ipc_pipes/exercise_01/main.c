#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define MSG_SIZE 12

int main(int argc, char* argv[]) {
    char read_buffer[MSG_SIZE];
    char *msg_1 = "Hello world!"; // Message to be sent through pipe
    int num_read = 0; // Number of bytes read
    int fds[2]; // File descriptors for pipe (fds[0] - read, fds[1] - write)
    pid_t child_pid; // Child process ID

    // Create a pipe
    if (pipe(fds) == -1) {
        handle_error("pipe()");
    }

    child_pid = fork();
    if (child_pid == 0) { // child process -> fork() returns 0
        printf("I am child process!\n");

        // Close write end of pipe in child
        if (close(fds[1]) == -1) {
            handle_error("close()");
        }

        // Read from pipe
        while(1) {
            num_read = read(fds[0], read_buffer, MSG_SIZE);
            if (num_read == -1) {
                handle_error("read()");
            } else if (num_read == 0) {
                // End of pipe (writer has closed)
                printf("pipe end-of-pipe\n");
                break;
            } else {
                // Print received message
                printf("Message from pipe: %s\n", read_buffer);
            }
        }

        // Close read end of pipe
        if (close(fds[0]) == -1) {
            handle_error("close()");
        }

        // Exit child process
        exit(EXIT_SUCCESS);
    } else if (child_pid > 0) { // parent process -> fork() returns PID of its child process
        printf("I am parent process!\n");

        // Close read end of pipe in parent
        if (close(fds[0]) == -1) {
            handle_error("close()");
        }

        // Write message to pipe
        if(write(fds[1], msg_1, MSG_SIZE) == -1) {
            handle_error("write()");
        }

        // Close write end of pipe (sends EOF to reader)
        if(close(fds[1]) == -1) {
            handle_error("close()");
        }

        // Wait for the child process to finish
        wait(NULL);
    } else { // error -> fork() returns -1
        handle_error("fork()");
    }

    return 0;
}