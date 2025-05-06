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
    int numBytes; // Number of bytes sent/received
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

    // Create a datagram socket (UDP): AF_INET = IPV4, SOCK_DGRAM = UDP, 0 = default protocol
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd == -1) { // Check for error
        handle_error("socket()");
    }

    // Write data to the server through the socket
    numBytes = sendto(server_fd, "Hello from client\n", strlen("Hello from client\n"), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
    if (numBytes == - 1) {
        handle_error("sendto()");
    }
    
    // Read data from the server through the socket and store it in read_buffer
    // (Tham số NULL để bỏ qua địa chỉ của người gửi)
    numBytes = recvfrom(server_fd, read_buffer, sizeof(read_buffer) - 1, 0, NULL, NULL);
    if (numBytes == -1) {
        handle_error("recvfrom()");
    }

    // Null-terminate the received string
    read_buffer[numBytes] = '\0';

    // Print the received data from the server
    printf("Data from server: %s\n", read_buffer);

    // Close the socket
    close(server_fd);
 
    return 0;
}