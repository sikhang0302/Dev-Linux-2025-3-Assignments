#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> // For perror()
#include <sys/socket.h> // For socket(), bind(), listen(), accept()
#include <netinet/in.h> // For sockaddr_in, htons(), ntohs()
#include <arpa/inet.h> // For inet_addr(), INADDR_ANY
#include <unistd.h> // For close(), write()

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[]) {
    int port_no; // Port number of the server
    int server_fd; // Socket file descriptor for communication with the server
    struct sockaddr_in server_addr; // Structure to store the server address information
    char read_buffer[100]; // Buffer to store the data read from the server
    // Initialize server_addr to avoid garbage values
    memset(&server_addr, 0, sizeof(struct sockaddr_in));

    // Check if the command-line arguments are provided (server address and port number)
    if (argc < 3) {
        printf("command: ./client <server address> <port number>\n");
        exit(EXIT_FAILURE);
    } else {
        port_no = atoi(argv[2]); // Convert the port number from string to integer
    }

    // Set up the server address structure
    server_addr.sin_family = AF_INET; // Use IPV4
    server_addr.sin_port = htons(port_no); // Convert the port number to network byte order
    // Convert the IP address from string to binary format
    // (Địa chỉ IP trong mạng được sử dụng dưới dạng nhị phân)
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) == -1) { // Check for error
        handle_error("inet_pton()");
    }

    // Create a stream socket (TCP): AF_INET = IPV4, SOCK_STREAM = TCP, 0 = default protocol
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) { // Check for error
        handle_error("socket()");
    }

    // Connect to the server using the address information stored in server_addr
    if (connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        handle_error("connect()");
    }

    // Write data to the server through the socket
    write(server_fd, "Hello from client\n", sizeof("Hello from client\n"));
    // Read data from the server through the socket and store it in read_buffer
    read(server_fd, read_buffer, sizeof(read_buffer));
    // Print the received data from the server
    printf("Data from server: %s\n", read_buffer);
    // Close the server connection
    close(server_fd);
    
    while(1);
 
    return 0;
}