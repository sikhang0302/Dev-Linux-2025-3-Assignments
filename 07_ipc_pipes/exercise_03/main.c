#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h> // Time functions for nanosleep()
#include <ctype.h> // Character handling functions

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

// Function to read user input dynamically    
char* read_user_input() {
    int capacity = 16; // Initial buffer capacity
    char *buffer = malloc(capacity);
    if (buffer == NULL) {
        return NULL;
    }
    int length = 0;
    int character;

    // Read characters until newline or EOF
    while ((character = getchar()) != '\n' && character != EOF) {
        // Resize buffer if needed
        if(length + 1 >= capacity) {
            capacity *= 2;
            char* temp = realloc(buffer, capacity);
            if(temp == NULL) {
                free(buffer);
                return NULL;
            }
            buffer = temp;
        }
        buffer[length] = character;
        length++;
    }

    // Null-terminate the string
    buffer[length] = '\0';

    // Reallocate to exact size needed
    char *final_buffer = realloc(buffer, length + 1);
    if (final_buffer == NULL) {
        return buffer;
    } 
    return final_buffer;
}

// Function to sleep for 1 millisecond
void one_ms_sleep() {
    struct timespec req = {
        .tv_sec = 0,
        .tv_nsec = 1000000  // 1 millisecond in nanoseconds
    };
    nanosleep(&req, NULL);
}

int main (int argc, char *argv[]) {
    // File descriptors for pipe (fds[0]=read, fds[1]=write)
    int fds[2];
    // Child process ID
    pid_t child_pid;
    // Length of strings being sent
    size_t user_input_length = 0; 
    
    // Create pipe
    if(pipe(fds) == -1) {
        handle_error("pipe()");
    }
    
    child_pid = fork();
    if (child_pid == 0) { // Child process
        int num_read_length = 0; // Stores return value from read()

        printf("Child Process\n");

        // Close write end of pipe in child
        if (close(fds[1]) == -1) { 
            handle_error("close()");
        }

        size_t received_string_length = 0; // Length of incoming string
        char *read_buffer; // Buffer for incoming string

        while(1) {
            // Read the length of the incoming string
            num_read_length = read(fds[0], &received_string_length, sizeof(size_t));

            if (num_read_length == -1) {
                handle_error("read()");
            } else if (num_read_length == 0) { // EOF - Parent closed write end
                printf("pipe end of pile\n");
                break;
            } else {
                // Allocate memory for the string
                read_buffer = (char *)malloc(received_string_length);
                if (read_buffer == NULL) {
                    handle_error("malloc()");
                }
                
                // Read the string content
                if(read(fds[0], read_buffer, received_string_length) == -1) {
                    free(read_buffer);
                    handle_error("read()");
                }

                // Print received string and number of characters in the string
                if (strlen(read_buffer) != 0) {
                    printf("String from parent process: %s\n", read_buffer);
                } else {
                    printf("String from parent process: N.A\n");
                }
                printf("Number of characters in received string: %d\n\n", (int)strlen(read_buffer));
        
                // Free memory for next string
                free(read_buffer);
            }
        }

        // Close read end of pipe
        if(close(fds[0]) == -1) {
            handle_error("close()");
        }

        // Exit child process
        exit(EXIT_SUCCESS);
    } else if (child_pid > 0) { // Parent process
        printf("Parent Process\n");

        // Small delay to ensure child process starts first
        one_ms_sleep();

        // Close read end of pipe in parent
        if (close(fds[0]) == -1) {
            handle_error("close()");
        }
        
        int choice = 0; // Menu choice storage
        char character; // Used for input validation
        
        while(1) {
            // Main menu loop
            printf("==================== MENU ====================\n");
            printf("1. Count number of characters in a string\n");
            printf("0. Exit\n");
            printf("Your choice?\n");

            // Read user choice
            int input_result = scanf("%d", &choice);

            if (input_result == EOF) {
                // EOF received
                printf("EOF received. Exit the program...\n");
                exit(EXIT_SUCCESS);
            } else if (input_result != 1) {
                // Clear buffer if non-numeric input
                while((character = getchar()) != '\n') {
                    if (character == EOF) { // EOF received
                        printf("\nEOF received. Exit the program...\n");
                        exit(EXIT_SUCCESS);
                    }
                }
                // Mark as invalid choice
                choice = -1; 
            } else { // Check for valid integer input
                while((character = getchar()) != '\n') {
                    if (character == EOF) {
                        // EOF received
                        printf("\nEOF received. Exit the program...\n");
                        exit(EXIT_SUCCESS);
                    }
                    
                    if(!isspace(character)) {
                        // Clear buffer if non-whitespace characters found
                        while((character = getchar()) != '\n' && character != EOF);
                        // Mark as invalid choice
                        choice = -1; 
                        break;
                    }
                }
            }
            printf("==============================================\n");

            // Process user choice
            switch(choice) {
                case 1: // Count characters in string
                {
                    printf("Enter a string: ");
                    char *user_input = read_user_input();
                    if(user_input != NULL) {
                        // Send string length to child
                        user_input_length = strlen(user_input) + 1; // Calculate length including null terminator
                        if(write(fds[1], &user_input_length, sizeof(size_t)) == -1) {
                            free(user_input);
                            handle_error("write()");
                        }

                        // Send string content to child
                        if(write(fds[1], user_input, user_input_length) == -1) {
                            free(user_input);
                            handle_error("write()");
                        }

                        // Free the input buffer
                        free(user_input);
                    }

                    // Small delay to ensure child process receives data
                    one_ms_sleep();

                    // Check for EOF condition
                    if (feof(stdin)) {
                        printf("EOF received. Exit the program...\n");
                        exit(EXIT_SUCCESS);
                    }

                    break;
                }
                case 0: // Exit program
                {
                    printf("Exit the program...\n");
                    // Close write end to signal child
                    if(close(fds[1]) == -1) {
                        handle_error("close()");
                    }
                    wait(NULL);      // Wait for child to exit
                    exit(EXIT_SUCCESS);
                }
                default: // Invalid choice
                {
                    printf("Invalid choice! Please try again.\n\n");
                    break;
                }
            }
        }
    } else { // error -> fork() returns -1
        handle_error("fork()");
    }

    return 0;
}