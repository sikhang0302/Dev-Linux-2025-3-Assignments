#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> // For perror()
#include <sys/socket.h> // For socket(), bind(), listen(), accept()
#include <sys/un.h> // For UNIX domain sockets
#include <unistd.h> // For close(), write()

#define BUFF_SIZE 100
#define SOCK_PATH "./sock_stream" // Path to server's socket file
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[]) {
    int server_fd; // Socket file descriptor
    struct sockaddr_un server_addr; // Server address structure
    char read_buffer[BUFF_SIZE]; // Buffer for received data

    // Initialize server address structure to zero
    memset(&server_addr, 0, sizeof(struct sockaddr_un));

    // Configure server address structure
    server_addr.sun_family = AF_UNIX; // UNIX domain socket
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);
    // Ensure null-termination
    server_addr.sun_path[sizeof(server_addr.sun_path) - 1] = '\0';

    // Create a UNIX domain stream socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        handle_error("socket()");
    }

    // Connect to the server
    if (connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        handle_error("connect()");
    }
    printf("Client: Connected to server\n");

    // Send message to server
    write(server_fd, "Hello from client\n", sizeof("Hello from client\n"));

    // Receive response from server
    read(server_fd, read_buffer, sizeof(read_buffer));

    // Print response from server
    printf("Data from server: %s\n", read_buffer);

    // Close the socket connection
    close(server_fd);
    
    return 0;
 }