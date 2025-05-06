#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> // For perror()
#include <sys/socket.h> // For socket(), bind(), listen(), accept()
#include <sys/un.h> // For UNIX domain sockets
#include <unistd.h> // For close(), write()

#define LISTEN_BACKLOG 50 // Maximum number of pending connections the server can queue
#define BUFF_SIZE 100
#define SOCK_PATH "./sock_stream" // Socket file path
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[]) {
    int len; // Length of client address structure
 
    int server_fd; // Main server socket file descriptor
    int new_socket_fd; // Client connection socket descriptor

    struct sockaddr_un server_addr; // Server address structure
    struct sockaddr_un client_addr; // Client address structure

    char read_buffer[100] = { 0 }; // Initialize read buffer with zeros

    // Clear server and client address structures
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    memset(&client_addr, 0, sizeof(struct sockaddr_un));

    // Create a UNIX domain stream socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        handle_error("socket()");
    }

    // Remove socket file if it already exists
    unlink(SOCK_PATH);

    // Configure server address structure
    server_addr.sun_family = AF_UNIX; // UNIX domain socket
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);
    // Ensure null-termination
    server_addr.sun_path[sizeof(server_addr.sun_path) - 1] = '\0';

    // Bind the socket to the server address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        handle_error("bind()");
    }

    // Mark the socket as listening with backlog queue size
    if (listen(server_fd, LISTEN_BACKLOG) == -1) {
        handle_error("listen()");
    }

    // Get size of client address structure
    len = sizeof(client_addr);

    while(1) {
        printf("Start listening on server...\n");

        // Accept incoming connection (blocks until client connects)
        new_socket_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&len);
        if (new_socket_fd == -1) {
            handle_error("accept()");
        }
        printf("Server: got connection\n");

        // Read data from client
        read(new_socket_fd, read_buffer, sizeof(read_buffer));

        // Send response to client
        write(new_socket_fd, "Hello from server\n", sizeof("Hello from server\n"));

        // Close client connection socket
        close(new_socket_fd);

        // Print received data
        printf("Data from client: %s\n", read_buffer);
    }

    // Close server socket
    close(server_fd);

    // Remove socket file
    if (remove(SOCK_PATH)) {
        handle_error("remove()");
    }

    return 0;
}