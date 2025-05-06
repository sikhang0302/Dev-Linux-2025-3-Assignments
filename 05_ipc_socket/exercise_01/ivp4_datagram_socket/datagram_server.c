#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> // For perror()
#include <sys/socket.h> // For socket(), bind(), listen(), accept()
#include <netinet/in.h> // For sockaddr_in, htons(), ntohs()
#include <arpa/inet.h> // For inet_addr(), INADDR_ANY
#include <unistd.h> // For close(), write()

#define BUFF_SIZE 100
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[]) {
    int port_no; // Port number for the server
    int len; // Length of client address structure
    int opt = 1; // Option value for setsockopt

    int server_fd; // Socket file descriptor
    int numBytes; // Number of bytes received/sent

    struct sockaddr_in server_addr; // Address information of server
    struct sockaddr_in client_addr; // Address information of client

    char read_buffer[BUFF_SIZE]; // Buffer to store received data

    // Command line argument: port number
    if (argc < 2) { // Check if port number is passed as an argument
        printf("No port provided\ncommand: ./server <port number>\n");
        exit(EXIT_FAILURE);
    } else {
        port_no = atoi(argv[1]); // Convert the port number from string to integer
    }
    // Initialize address structures to avoid garbage values
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    memset(&client_addr, 0, sizeof(struct sockaddr_in));

    // -------------------- 01. Create socket --------------------
    // Create a datagram socket (UDP): AF_INET = IPV4, SOCK_DGRAM = UDP, 0 = default protocol
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd == -1) { // Check for error
        handle_error("socket()");
    }

    // -------------------- 01.1 Set socket options --------------------
    // enable address and port reuse to avoid "Adress already in use" error
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        handle_error("setsockopt()");
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
        handle_error("setsockopt()");
    }

    // -------------------- 01.2 Initialize server information --------------------
    server_addr.sin_family = AF_INET; // Use IPV4
    server_addr.sin_port = htons(port_no); // Convert port value to network byte order (big edian)
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections on any network interface
                                              // or use inet_addr("192.168.80.130"); for specific IP

    // -------------------- 02. Bind --------------------                                          
    // Bind the socket to the specified IP and port
    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) { // Check for error
        handle_error("bind()");
    }

    // Calculate the initial size of the client address structure
    len = sizeof(client_addr);
    // Print a message indicating that the server is ready to receive data
    printf("Server is listening at port: %d\n...\n", ntohs(server_addr.sin_port));

    // Server loop
    while(1) {
        // Read a message from the client
        numBytes = recvfrom(server_fd, read_buffer, BUFF_SIZE - 1, 0, (struct sockaddr*)&client_addr, (socklen_t *)&len);
        if (numBytes == -1) { // Check for error
            handle_error("recvfrom()");
        }
        
        // Null-terminate the received string
        read_buffer[numBytes] = '\0'; 

        // Print the received data from the client
        printf("Data from client: %s\n", read_buffer);

        // Send a message to the client
        numBytes = sendto(server_fd, "Hello from server\n", strlen("Hello from server\n"), 0, (struct sockaddr*)&client_addr, len);
        if (numBytes == -1) { // Check for error
            handle_error("sendto()");
        }
    }

    // Close socket
    close(server_fd);

    return 0;
}