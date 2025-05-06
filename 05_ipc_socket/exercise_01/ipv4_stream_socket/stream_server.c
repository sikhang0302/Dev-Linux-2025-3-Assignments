#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> // For perror()
#include <sys/socket.h> // For socket(), bind(), listen(), accept()
#include <netinet/in.h> // For sockaddr_in, htons(), ntohs()
#include <arpa/inet.h> // For inet_addr(), INADDR_ANY
#include <unistd.h> // For close(), write()

#define LISTEN_BACKLOG 50 // Maximum number of pending connections the server can queue
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[]) {
    int port_no; // Port number for the server
    int len; // Length of client address structure
    int opt = 1; // Option value for setsockopt

    int server_fd; // Listening socket file descriptor
    int new_socket_fd; // File descriptor for newly accepted client connection

    struct sockaddr_in server_addr; // Address information of server
    struct sockaddr_in client_addr; // Address information of client

    char read_buffer[100]; // Buffer to store the data read from the client

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
    // Create a stream socket (TCP): AF_INET = IPV4, SOCK_STREAM = TCP, 0 = default protocol
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
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
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to all available network interfaces
                                              // or use inet_addr("192.168.80.130"); for specific IP

    // -------------------- 02. Bind --------------------                                          
    // Bind the socket to the specified IP and port
    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) { // Check for error
        handle_error("bind()");
    }

    // -------------------- 03. Listen --------------------
    // Mark the socket as passive — ready to accept incoming connection requests
    if(listen(server_fd, LISTEN_BACKLOG) == -1) { // Check for error
        handle_error("listen()");
    }

    // -------------------- 04. Accept connections in loop --------------------
    // Calculate the size of the client address structure before using accept()
    len = sizeof(client_addr);
    // Infinite loop: server stays alive and handles one client at a time
    while(1) {
        printf("Server is listening at port: %d\n...\n", ntohs(server_addr.sin_port));
        // Accept a client connection 
        // On success, returns a new socket FD for communication with the client
        new_socket_fd = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t *)&len);
        if (new_socket_fd == -1) { // Check for error
            handle_error("accept()");
        }

        // A client has connected
        printf("Server: got connection\n");
        // Read a message from the client
        read(new_socket_fd, read_buffer, sizeof(read_buffer));
        // Send a message to the client
        write(new_socket_fd, "Hello from server\n", sizeof("Hello from server\n"));
        // Close the client connection
        close(new_socket_fd);
        // Print the received data from the client
        printf("Data from client: %s\n", read_buffer);
    }
    // Close the listening socket
    close(server_fd);

    return 0;
}

/************************************************************************************************************************************************ 
1. setsockopt
--> Là hàm dùng để thiết lập tùy chọn cho socket
--> Nguyên mẫu hàm: int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);

2. setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) 
   setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))
--> server_fd
    --> Là file descriptor của socket server đã tạo bằng socket()
--> SOL_SOCKET
    --> Là mức áp dụng cho tùy chọn chung của socket 
    --> Phải chỉ định đúng level tương ứng với optname
    --> Ví dụ: level = SOL_SOCKET tương ứng với các optname như SO_REUSEADDR, SO_REUSEPORT,...
--> SO_REUSEPORT và SO_REUSEADDR
    --> Là các tùy chọn muốn thiết lập: 
        --> SO_REUSEADDR: Cho phép bind vào địa chỉ đang ở trạng thái TIME_WAIT (khi socket server vừa được đóng)
            --> Khi một server vừa tắt, socket của nó có thể vẫn còn nằm trong trạng thái TIME_WAIT 
            --> Khi socket còn ở TIME_WAIT, nếu  bind lại vào cùng địa chỉ + cổng, nó sẽ bị từ chối, trừ khi có SO_REUSEADDR.
            --> Dùng SO_REUSEADDR để nói với kernel: "Tôi biết địa chỉ này đang bị chiếm, nhưng cho tôi dùng lại nó đi"
            --> Tránh phải chờ vài phút mới restart server được
        --> SO_REUSEPORT: Cho phép nhiều socket cùng bind vào cùng một địa chỉ và cổng
            --> Mỗi thread/process có thể có 1 socket riêng bind cùng địa chỉ+cổng
            --> Khi có nhiều socket bind vào cùng cổng với SO_REUSEPORT, kernel sẽ phân phối kết nối đến từng socket
--> opt
    --> Thường là một biến kiểu int có giá trị 1, biểu thị là bật tùy chọn
--> sizeof(opt) 
    --> Kích thước của opt
    --> Giúp setsockopt() biết cần đọc bao nhiêu byte từ opt
************************************************************************************************************************************************/