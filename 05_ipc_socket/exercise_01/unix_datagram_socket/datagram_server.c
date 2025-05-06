#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> // For perror()
#include <sys/socket.h> // For socket(), bind(), listen(), accept()
#include <sys/un.h> // For UNIX domain sockets
#include <unistd.h> // For close(), write()

#define BUFF_SIZE 100  
#define SOCK_PATH "./sock_dgram" // Path for the UNIX domain socket
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[]) {
    int len; // Length of client address structure

    int server_fd; // Server socket file descriptor
    int numBytes; // Number of bytes received/sent
 
    struct sockaddr_un server_addr; // Server address structure
    struct sockaddr_un client_addr; // Client address structure

    char read_buffer[BUFF_SIZE];

    // Initialize server and client address structures to zero
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    memset(&client_addr, 0, sizeof(struct sockaddr_un));

    // Create a UNIX domain datagram socket
    server_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (server_fd == -1) {
        handle_error("socket()");
    }

    // Remove the socket file if it already exists
    unlink(SOCK_PATH);

    // Configure server address structure
    server_addr.sun_family = AF_UNIX; // UNIX domain socket
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);
    // Ensure null-termination
    server_addr.sun_path[sizeof(server_addr.sun_path) - 1] = '\0'; 

    // Bind the socket to the server address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        handle_error("bind()");
    };
    printf("Start listening on server...\n");

    // Get size of client address structure
    len = sizeof(client_addr);

    while(1) {
        // Receive data from client
        numBytes = recvfrom(server_fd, read_buffer, BUFF_SIZE - 1, 0, (struct sockaddr *)&client_addr, (socklen_t *)&len);
        if (numBytes == -1) {
            handle_error("recvfrom()");
        }

        // Null-terminate the received data
        read_buffer[numBytes] = '\0';

        // Print received data
        printf("Data from client: %s\n", read_buffer);

        // Send response to client
        numBytes = sendto(server_fd, "Hello from server\n", strlen("Hello from server\n"), 0, (struct sockaddr *)&client_addr, (socklen_t)len);
        if (numBytes == -1) {
            handle_error("sendto()");
        }
    }

    // Close the socket
    close(server_fd);

    // Remove the socket file
    if(remove(SOCK_PATH)) {
        handle_error("remove()");
    }

    return 0;
}