#include "../inc/chat_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <time.h>
#include <termios.h>
#include <errno.h>

// Global variables
peer_t peers[MAX_PEERS];                                     // Array to store connected peers
int peer_count = 0;                                          // Current number of connected peers
char server_ip[IPV4_ADDRSTRLEN];                             // IP address of the server
pthread_mutex_t peer_mutex = PTHREAD_MUTEX_INITIALIZER;      // Mutex for thread-safe access to peers
pthread_t server_tid;                                        // Thread ID for the server thread

// Sleep for the specified milliseconds
void sleep_in_ms(int ms) {
    struct timespec req = {
        .tv_sec = 0,
        .tv_nsec = ms * 1000000  // Convert milliseconds to nanoseconds
    };
    nanosleep(&req, NULL);
}

// Clear the input buffer
void clear_input_buffer() {
    tcflush(STDIN_FILENO, TCIFLUSH);
}

// Get the real IP address of the machine (skip loopback (127/8))
int get_real_ip(char* ip_buffer) {
    struct ifaddrs *ifaddr, *ifa;
    int result = -1; // Default to error
    
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    // Iterate through all interfaces
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET && 
            strcmp(ifa->ifa_name, "lo") != 0) { // Skip loopback interface
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            strcpy(ip_buffer, inet_ntoa(sa->sin_addr));
            result = 0;
            break;
        }
    }
    freeifaddrs(ifaddr);
    return result;
}

// Server thread: Handles incoming connections
void *server_thread(void *arg) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int port_no_received = *((int *)arg);
    
    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        
        exit(EXIT_FAILURE);
    }
    
    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_port = htons(port_no_received);

    // Get the real IP of the server
    char real_ip[16];
    if (get_real_ip(real_ip) == 0) {
        inet_pton(AF_INET, real_ip, &address.sin_addr);
        strcpy(server_ip, real_ip);  // Store the IP globally
    } else {
        clear_input_buffer();
        printf("Warning: Using INADDR_ANY (could not find real IP)\n");
        address.sin_addr.s_addr = INADDR_ANY;
    }
    
    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Start listening for connections
    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server IP: %s, listening on port %d\n", real_ip, port_no_received);
    
    // Accept incoming connections
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }

        // Read the listening port from the client
        char buffer[BUFFER_SIZE];
        int valread = read(new_socket, buffer, BUFFER_SIZE - 1);
        int peer_listen_port = 0;
        
        if (valread > 0) {
            buffer[valread] = '\0';
            if (sscanf(buffer, "LISTEN_PORT=%d", &peer_listen_port) != 1) {
                peer_listen_port = ntohs(address.sin_port); // Fallback to connection port
            }
        } else {
            peer_listen_port = ntohs(address.sin_port);
        }
        
        // Add the new peer to the list
        clear_input_buffer();
        printf("\n\nNew connection from %s:%d\n", 
               inet_ntoa(address.sin_addr), 
               peer_listen_port);
        
        pthread_mutex_lock(&peer_mutex);
        if (peer_count < MAX_PEERS) {
            peers[peer_count].id = peer_count; 
            peers[peer_count].socket = new_socket;
            peers[peer_count].address = address;
            peers[peer_count].listen_port = peer_listen_port; 
            peer_count++;
            printf("Peer added. Total peers: %d\n", peer_count);
        } else {
            printf("Max peers reached. Rejecting connection.\n");
            close(new_socket);
        }
        printf("\n> ");
        fflush(stdout); 
        pthread_mutex_unlock(&peer_mutex);
    }
    
    return NULL;
}

// Start the server thread
void start_server(int port_no) {
    if (pthread_create(&server_tid, NULL, server_thread, (void *)(&port_no)) != 0) {
        perror("pthread_create failed");
        exit(1);
    }
    sleep_in_ms(2); // Small delay to ensure thread starts
}

// Check if a connection is to the listening port (self-connection)
int is_self_connection(int port, int listen_port) {
    return (port == listen_port);
}

// Check if a connection to the specified IP and port already exists
int is_existing_connection(const char *ip, int port) {
    pthread_mutex_lock(&peer_mutex);
    for (int i = 0; i < peer_count; i++) {
        if (strcmp(inet_ntoa(peers[i].address.sin_addr), ip) == 0 && 
            (ntohs(peers[i].address.sin_port) == port || 
             peers[i].listen_port == port)) {
            pthread_mutex_unlock(&peer_mutex);
            return 1;
        }
    }
    pthread_mutex_unlock(&peer_mutex);
    return 0;
}

// Client role: Connect to another peer
void connect_to_peer(const char *ip, int port, int listen_port) {
    // Check for self-connection
    if (is_self_connection(port, listen_port)) {
        printf("Cannot connect to your own listening port %d\n", listen_port);
        return;
    }

    // Check for existing connection
    if (is_existing_connection(ip, port)) {
        printf("Already connected to %s:%d\n", ip, port);
        return;
    }

    int sock = 0;
    struct sockaddr_in serv_addr;
    
    // Create client socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        return;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    
    // Convert IP address to binary form
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        return;
    }
    
    // Connect to the peer
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect failed");
        return;
    }

    // Send the listening port to the peer
    char port_msg[50];
    sprintf(port_msg, "LISTEN_PORT=%d\n", listen_port);
    send(sock, port_msg, strlen(port_msg), 0);
    
    // Add the peer to the list
    pthread_mutex_lock(&peer_mutex);
    if (peer_count < MAX_PEERS) {
        peers[peer_count].id = peer_count; 
        peers[peer_count].socket = sock;
        peers[peer_count].address = serv_addr;
        peers[peer_count].listen_port = -1; // Listening port is not needed here
                                            // Assign to -1 for checking condition
        peer_count++;
        printf("Connected to peer %s:%d. Total peers: %d\n", ip, port, peer_count);
    } else {
        printf("Max peers reached. Cannot connect to new peer.\n");
        close(sock);
    }
    pthread_mutex_unlock(&peer_mutex);
}

// Send a message to a specific peer
void send_to_peer(int peer_id, const char *message) {
    pthread_mutex_lock(&peer_mutex);
    if (peer_id >= 0 && peer_id < peer_count) {
        if (peers[peer_id].socket > 0) {
            send(peers[peer_id].socket, message, strlen(message), 0);
            printf("Sent to peer [%d]: %s\n", peer_id, message);
        } else {
            printf("Peer [%d] is disconnected\n", peer_id);
        }
    } else {
        printf("Invalid peer ID: %d\n", peer_id);
    }
    pthread_mutex_unlock(&peer_mutex);
}

// Terminate a connection with a peer
void terminate_connection(int peer_id) {
    pthread_mutex_lock(&peer_mutex);

    if (peer_id < 0 || peer_id >= peer_count) {
        printf("Invalid peer ID: %d\n", peer_id);
        pthread_mutex_unlock(&peer_mutex);
        return;
    }

    if (peers[peer_id].socket < 0) {
        printf("Peer ID %d is already disconnected\n", peer_id);
        pthread_mutex_unlock(&peer_mutex);
        return;
    }

    // Close the connection and remove the peer
    printf("Terminating connection with peer [%d] %s:%d\n", 
           peers[peer_id].id,
           inet_ntoa(peers[peer_id].address.sin_addr),
           ntohs(peers[peer_id].address.sin_port));
    close(peers[peer_id].socket);
    peers[peer_id].socket = -1;

    // Shift the array to fill the gap
    for (int j = peer_id; j < peer_count - 1; j++) {
        peers[j] = peers[j+1];
    }
    peer_count--;

    pthread_mutex_unlock(&peer_mutex);
}

// Print the list of connected peers
void print_peers() {
    pthread_mutex_lock(&peer_mutex);
    peer_t temp_peers[MAX_PEERS];
    int temp_count = peer_count;
    memcpy(temp_peers, peers, (sizeof(peer_t) * temp_count)); 
    pthread_mutex_unlock(&peer_mutex);

    printf("Connected peers (%d):\n", temp_count);
    for (int i = 0; i < temp_count; i++) {
        printf("  [%d] %s:", temp_peers[i].id, inet_ntoa(temp_peers[i].address.sin_addr));
        if (temp_peers[i].listen_port != -1) {
            printf("%d\n", temp_peers[i].listen_port);
        } else {
            printf("%d\n", ntohs(temp_peers[i].address.sin_port));
        }
    }
}

// Handle communication with peers and user input
void handle_peer_communication(int listen_port, command_handler_t cmd_handler) {
    fd_set readfds;
    fd_set exceptfds;
    int max_sd;
    char buffer[BUFFER_SIZE];
    int need_prompt = 1; // Flag to check if prompt is needed
    
    while (1) {
        if (need_prompt) {
            printf("\n> ");
            fflush(stdout);
            need_prompt = 0;
        }

        FD_ZERO(&readfds);
        FD_ZERO(&exceptfds);
        
        // Add peer sockets and stdin to the fd_set
        pthread_mutex_lock(&peer_mutex);
        max_sd = 0;
        
        for (int i = 0; i < peer_count; i++) {
            int sd = peers[i].socket;
            if (sd > 0) {
                FD_SET(sd, &readfds);
                FD_SET(sd, &exceptfds);
                if (sd > max_sd) {
                    max_sd = sd;
                }
            }
        }
        pthread_mutex_unlock(&peer_mutex);
        
        FD_SET(STDIN_FILENO, &readfds);
        if (STDIN_FILENO > max_sd) {
            max_sd = STDIN_FILENO;
        }

        // Wait for activity on sockets or stdin
        // Set timeout for select() to 1 second
        struct timeval timeout;
        timeout.tv_sec = 1; // Seconds
        timeout.tv_usec = 0; // Microseconds

        int activity = select(max_sd + 1, &readfds, NULL, &exceptfds, &timeout);
        
        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        pthread_mutex_lock(&peer_mutex);
        for (int i = 0; i < peer_count; i++) {
            int sd = peers[i].socket;
            if (sd >= 0) {
                if (FD_ISSET(sd, &exceptfds)) { // Handle peer exceptions (errors)
                    clear_input_buffer();
                    printf("\n\nPeer [%d] connection error! Disconnecting...\n", peers[i].id);
                    close(sd);
                    peers[i].socket = -1;
                    
                    // Shift the array to fill the gap
                    for (int j = i; j < peer_count - 1; j++) {
                        peers[j] = peers[j+1];
                    }
                    peer_count--;
                    i--;
                    need_prompt = 1;
                } else if (FD_ISSET(sd, &readfds)) { // Handle peer disconnections
                    int valread = read(sd, buffer, BUFFER_SIZE - 1);
                    if (valread <= 0) {
                        clear_input_buffer();
                        printf("\n\nPeer [%d] disconnected: %s:", peers[i].id, 
                            inet_ntoa(peers[i].address.sin_addr));
                        if (peers[i].listen_port != -1) {
                            printf("%d\n", peers[i].listen_port);
                        } else {
                            printf("%d\n", ntohs(peers[i].address.sin_port));
                        }

                        close(sd);
                        peers[i].socket = -1;
                        
                        // Shift the array to fill the gap
                        for (int j = i; j < peer_count - 1; j++) {
                            peers[j] = peers[j+1];
                        }
                        peer_count--;
                        i--;
                        need_prompt = 1;
                    } else { // Handle incoming messages from peers
                        clear_input_buffer();
                        buffer[valread] = '\0';
                        printf("\n\nMessage: %s\n", buffer);
                        printf("Receive from peer [%d]: %s:", peers[i].id, 
                            inet_ntoa(peers[i].address.sin_addr));
                        if (peers[i].listen_port != -1) {
                            printf("%d\n", peers[i].listen_port);
                        } else {
                            printf("%d\n", ntohs(peers[i].address.sin_port));
                        }
                        need_prompt = 1;
                    }
                }
            }
        }
        pthread_mutex_unlock(&peer_mutex);
        
        // Handle user input
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
                break; // Exit on EOF
            }

            need_prompt = 1;

            // Process the command using the provided command handler
            ProgramState_t state = cmd_handler(buffer, listen_port, server_ip);
            if (state == EXIT_REQUESTED) {
                break;
            }
        }
    }
}

// Cleanup resources before exiting
void cleanup_chat_system() {
    pthread_mutex_lock(&peer_mutex);
    for (int i = 0; i < peer_count; i++) {
        if (peers[i].socket > 0) {
            close(peers[i].socket);
        }
    }
    pthread_mutex_unlock(&peer_mutex);
    
    pthread_cancel(server_tid);
    pthread_join(server_tid, NULL);
}