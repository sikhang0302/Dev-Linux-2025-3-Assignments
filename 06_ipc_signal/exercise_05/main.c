#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/select.h>
#include <string.h>
#include <errno.h>

#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)

// Signal handler for SIGINT
void handle_sigint(int sig) {
    printf("\nSIGINT received.\n");
}

// Signal handler for SIGTERM 
void handle_sigterm(int sig) {
    printf("\nSIGTERM received. Exiting program...\n");
    exit(EXIT_SUCCESS);
}

int main() {
     // Set up signal handlers using sigaction structure
    struct sigaction sa_int, sa_term;
    
    // Initialize and set up SIGINT handler
    memset(&sa_int, 0, sizeof(sa_int)); // Clear the structure
    sa_int.sa_handler = handle_sigint; // Set the handler function
    sigaction(SIGINT, &sa_int, NULL); // Register the handler
    
    // Initialize and set up SIGTERM handler
    memset(&sa_term, 0, sizeof(sa_term)); // Clear the structure
    sa_term.sa_handler = handle_sigterm; // Set the handler function
    sigaction(SIGTERM, &sa_term, NULL); // Register the handler 
    
    printf("Program is running. Press Ctrl+C to send SIGINT, or type 'exit' to quit.\n");
    
    while (1) {
        fd_set fds; // File descriptor set for select()
        FD_ZERO(&fds); // Clear the set
        FD_SET(STDIN_FILENO, &fds); // Add stdin to the set (STDIN_FILENO)
        
        // Set timeout for select() to 1 second
        struct timeval timeout;
        timeout.tv_sec = 1; // Seconds
        timeout.tv_usec = 0; // Microseconds
        
        // Wait for file descriptors to become ready with 1 second timeout
        // Only monitor stdin (file descriptor 0) for reading
        int ready = select(STDIN_FILENO + 1, &fds, NULL, NULL, &timeout);
        
        if (ready == -1) {
            if (errno == EINTR) { // If select was interrupted by a signal (EINTR), continue loop
                continue;
            } else {
                // For other errors, print error and exit
                handle_error("select");
            }
        }

        // Timeout occurred - no input or signals 
        if (ready == 0) {
            printf("Timeout!\n");
        }
        
        // Check if stdin has data available to read
        if (FD_ISSET(STDIN_FILENO, &fds)) {
            char buffer[256]; 

            // Read input
            if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                // Remove newline character
                buffer[strcspn(buffer, "\n")] = '\0';
                
                // Check if user wants to exit
                if (strcmp(buffer, "exit") == 0) {
                    printf("Exiting program...\n");
                    break; // Exit the main loop
                } else {
                    // Print the user input
                    printf("You entered: %s\n", buffer);
                }
            }
        }
    }
    
    return 0;
}