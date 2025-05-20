#ifndef CHAT_CORE_H
#define CHAT_CORE_H

#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <pthread.h>

// Constants for configuration
#define MAX_PEERS 10                                         // Maximum number of connected peers
#define BUFFER_SIZE 1024                                     // Buffer size for messages
#define MAX_MSG_LEN_ALLOW 100                                // Maximum allowed message length
#define EXCEEDED_MSG MAX_MSG_LEN_ALLOW + 2                   // Buffer size for exceeded messages
#define IPV4_ADDRSTRLEN 16                                   // Length of IPv4 address string

// Structure to store peer information
typedef struct {
    int id;                                                  // Peer ID (index in the array)
    int socket;                                              // Socket descriptor for the peer
    struct sockaddr_in address;                              // IP address and port of the peer
    int listen_port;                                         // Listening port of the peer (if known)
} peer_t;

// Enum to represent program state
typedef enum {
    EXIT_REQUESTED,                                          // Request to exit the program
    KEEP_RUNNING                                             // Program continues running
} ProgramState_t;

// Function pointer type for command handlers
typedef ProgramState_t (*command_handler_t)(const char* command, int listen_port, const char *server_ip);

// Core function prototypes
int get_real_ip(char* ip_buffer);                            // Get the real IP address of the machine
void start_server(int port_no);                              // Start server thread
void handle_peer_communication(int listen_port,              // Handle peer communication and user input
    command_handler_t cmd_handler);
void connect_to_peer(const char *ip, int port,               // Connect to another peer
    int listen_port);   
void send_to_peer(int peer_id, const char *message);         // Send a message to a specific peer
void terminate_connection(int peer_id);                      // Terminate a connection with a peer
void print_peers();                                          // Print the list of connected peers
void cleanup_chat_system();                                  // Cleanup resources before exiting

// Utility function prototypes  
void sleep_in_ms(int ms);                                    // Sleep for milliseconds
void clear_input_buffer();                                   // Clear the input buffer

#endif