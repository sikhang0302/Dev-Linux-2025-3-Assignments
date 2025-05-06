#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define BUFF_SIZE 256

int main (int argc, char* argv[]) {
    pid_t child_pid_1; // Process ID for first child
    pid_t child_pid_2; // Process ID for second child

    int fds_pipe_1[2]; // Pipe 1: Parent → Child 1 (fds_pipe_1[0] for read, fds_pipe_1[1] for write)
    int fds_pipe_2[2]; // Pipe 2: Child 1 → Child 2 (fds_pipe_2[0] for read, fds_pipe_2[1] for write)

    char read_buffer[BUFF_SIZE];

    // Create pipe 1 (Parent to Child 1)
    if(pipe(fds_pipe_1) == -1) {
        handle_error("pipe()");
    }
    // Create pipe 2 (Child 1 to Child 2)
    if(pipe(fds_pipe_2) == -1) {
        handle_error("pipe()");
    }

    child_pid_1 = fork();
    if (child_pid_1 == 0) { // First child process
        // Close write end of Pipe 1 (Child 1 only reads from Parent)
        if (close(fds_pipe_1[1]) == -1) {
            handle_error("close()");
        }
        // Close read end of Pipe 2 (Child 1 only writes to Child 2)
        if (close(fds_pipe_2[0]) == -1) {
            handle_error("close()");
        }

        while (1) {
            // Read message from Parent through Pipe 1
            int num_read = read(fds_pipe_1[0], read_buffer, BUFF_SIZE);
            if (num_read == -1) {
                handle_error("read()");
            } else if(num_read == 0) { // EOF - Parent closed write end
                printf("pipe end-of-pipe\n");
                break;
            } else {
                // Null-terminate the received message
                read_buffer[num_read] = '\0';
                printf("Message received by Child 1: %s\n", read_buffer);

                // Modify message by appending another string
                strcat(read_buffer, " Handled by Child 1");

                // Forward modified message to Child 2 through Pipe 2
                if(write(fds_pipe_2[1], read_buffer, strlen(read_buffer) + 1) == -1) {
                    handle_error("write()");
                }
            }
        }

        // Close remaining pipe ends
        if(close(fds_pipe_1[0]) == -1) {
            handle_error("close()");
        }
        if(close(fds_pipe_2[1]) == -1) {
            handle_error("close()");
        }
        
        // Exit child process
        exit(EXIT_SUCCESS);
    } else if (child_pid_1 > 0) { // Parent process
        child_pid_2 = fork();
        if (child_pid_2 == 0) { // Second child process
            // Close write end of Pipe 2 (Child 2 only reads from Child 1)
            if (close(fds_pipe_2[1]) == -1) {
                handle_error("close()");
            }
            // Close write end of Pipe 1
            if (close(fds_pipe_1[1]) == -1) { 
                handle_error("close()");
            }
            // Close read end of Pipe 1
            if (close(fds_pipe_1[0]) == -1) {
                handle_error("close()");
            }

            while (1) {
                // Read message from Child 1 through Pipe 2
                int num_read = read(fds_pipe_2[0], read_buffer, BUFF_SIZE);
                if (num_read == -1) {
                    handle_error("read()");
                } else if (num_read == 0) { // EOF - Child 1 closed write end
                    printf("pipe end-of-pipe\n");
                    break;
                } else {
                    // Print message from Child 1 through Pipe 2
                    printf("Message received by Child 2: %s\n", read_buffer);
                }
            }

            // Close remaining pipe end
            if (close(fds_pipe_2[0]) == -1) { 
                handle_error("close()");
            }

            // Exit child process
            exit(EXIT_SUCCESS);
        } else if (child_pid_2 > 0) { // Parent process
            // Close read end of Pipe 1 (Parent only writes to Child 1)
            if (close(fds_pipe_1[0]) == -1) {
                handle_error("close()");
            }
            // Close write end of Pipe 2
            if (close(fds_pipe_2[1]) == -1) {
                handle_error("close()");
            }
            // Close read end of Pipe 2
            if (close(fds_pipe_2[0]) == -1) {
                handle_error("close()");
            }
            
            // Message to send
            char *msg = "Hello from Parent process.";
            printf("Message sent by Parent process: %s\n", msg);

            // Send message to Child 1 through Pipe 1
            if(write(fds_pipe_1[1], msg, strlen(msg) + 1) == -1) {
                handle_error("write()");
            }

            // Close write end of Pipe 1 (send EOF to Child 1)
            if(close(fds_pipe_1[1]) == -1) {
                handle_error("close()");
            }

            // Wait for both children to complete
            waitpid(child_pid_1, NULL, 0);
            waitpid(child_pid_2, NULL, 0);
        } else { // error -> fork() returns -1
            handle_error("fork()");
        }
    } else { // error -> fork() returns -1
        handle_error("fork()");
    }

    return 0;
}