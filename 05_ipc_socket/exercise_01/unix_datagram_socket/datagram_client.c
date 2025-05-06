#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> // For perror()
#include <sys/socket.h> // For socket(), bind(), listen(), accept()
#include <sys/un.h> // For UNIX domain sockets
#include <unistd.h> // For close(), write()

#define BUFF_SIZE 100
#define SOCK_PATH "./sock_dgram" // Path to the server's socket file
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[]) {
    int server_fd; // Socket file descriptor
    struct sockaddr_un server_addr; // Server address structure
    char read_buffer[BUFF_SIZE]; 
    int numBytes; // Number of bytes sent/received
    int len; // Length of address structure
    int optval = 1; // Option value for setsockopt

    // Initialize server address structure to zero
    memset(&server_addr, 0, sizeof(struct sockaddr_un));

    // Create a UNIX domain datagram socket
    server_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(server_fd == -1) {
        handle_error("socket()");
    }

    // Configure server address structure
    server_addr.sun_family = AF_UNIX; // UNIX domain socket
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);
    // Ensure null-termination
    server_addr.sun_path[sizeof(server_addr.sun_path) - 1] = '\0';

    /***********************************************************************************************************
     * SO_PASSCRED: Là tùy chọn socket cho phép truyền thông tin xác thực người dùng từ client đến server.     *
     * Khi tùy chọn này được bật, mỗi khi một tiến trình (client) gửi dữ liệu qua socket UNIX, hệ thống sẽ     *
     * gửi kèm thông tin xác thực về tiến trình đó (như UID và GID). Điều này giúp cho server biết được ai     *
     * là người gửi yêu cầu hoặc khi cần kiểm tra quyền truy cập.                                              *
     * Trường hợp này sử dụng SOL_PASSCRED giúp server biết được cần gửi dữ liệu đến client nào khi sử dụng    *
     * hàm sendto() bên phía server, tránh được lỗi "Transport endpoint is not connected".                     *
     ***********************************************************************************************************/
    setsockopt(server_fd, SOL_SOCKET, SO_PASSCRED, &optval, sizeof(optval)); 
    
    // Get size of server address structure
    len = sizeof(server_addr);

    // Send a message to the server
    numBytes = sendto(server_fd, "Hello from client\n", strlen("Hello from client\n"), 0, (struct sockaddr *)&server_addr, (socklen_t)len);
    if (numBytes == -1) {
        handle_error("sendto()");
    }

    // Receive response from server
    numBytes = recvfrom(server_fd, read_buffer, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, (socklen_t *)&len);
    if (numBytes == -1) {
        handle_error("recvfrom()");
    }

    // Null-terminate the received data
    read_buffer[numBytes] = '\0';

    // Print the response from server
    printf("Data from server: %s\n", read_buffer);

    // Close the socket
    close(server_fd);

    return 0;
}