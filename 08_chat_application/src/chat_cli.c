#include "../inc/chat_cli.h"
#include <stdio.h>
#include <string.h>

// Display help information for available commands
void show_help() {
    printf("Use the commands below:\n");
    printf("1. help                                 : display user interface options\n");
    printf("2. myip                                 : display IP address of this app\n");
    printf("3. myport                               : display listening port of this app\n");
    printf("4. connect <destination> <port>         : connect to the app of another computer\n");
    printf("5. list                                 : list all the connections of this app\n");
    printf("6. terminate <connection id>            : terminate a connection\n");
    printf("7. send <connection id> <message>       : send a message to a connection\n");
    printf("8. exit                                 : close all connections and terminate this app\n");
}

// Process user commands and return the program state
ProgramState_t process_command(const char *buffer, int listen_port, const char *server_ip) {
    if (strncmp(buffer, "exit", 4) == 0) {
        return EXIT_REQUESTED; // Exit the program if "exit" command is received
    } else {
        printf("\n");

        if (strncmp(buffer, "help", 4) == 0) { // Handle "help" command
            show_help();
        } else if (strncmp(buffer, "myip", 4) == 0) { // Handle "myip" command
            printf("IP address of this app: %s\n", server_ip);
        } else if (strncmp(buffer, "myport", 6) == 0) { // Handle "myport" command
            printf("Listening port of this app: %d\n", listen_port);
        } else if (strncmp(buffer, "send", 4) == 0) { // Handle "send" command
            int peer_id;
            char message[EXCEEDED_MSG] = {0};
            if (sscanf(buffer, "send %d %101[^\n]", &peer_id, message) >= 1) {
                if (strlen(message) > MAX_MSG_LEN_ALLOW) {
                    printf("Warning: Message has exceeded 100 characters and was truncated!\n");
                    message[strlen(message) - 1] = '\0';
                }
                // Send the message to the specified peer
                send_to_peer(peer_id, message); 
            } else { 
                printf("Invalid send command. Usage: send <connection id> <message>\n");
            }
        } else if (strncmp(buffer, "connect", 7) == 0) { // Handle "connect" command
            char ip[16];
            int port;
            if (sscanf(buffer, "connect %15s %d", ip, &port) == 2) {
                // Connect to the specified peer
                connect_to_peer(ip, port, listen_port);
            } else {
                printf("Invalid connect command. Usage: connect <destination> <port>\n");
            }
        } else if (strncmp(buffer, "list", 4) == 0) { // Handle "list" command
            // Print the list of connected peers
            print_peers();
        } else if (strncmp(buffer, "terminate", 9) == 0) { // Handle "terminate" command
            int peer_id;
            if (sscanf(buffer, "terminate %d", &peer_id) == 1) {
                // Terminate the specified connection
                terminate_connection(peer_id);
            } else {
                printf("Invalid terminate command. Usage: terminate <connection id>\n");
            }
        } else {
            // Check for invalid command
            printf("Invalid command! Please try again\n");
        }
        // Continue running the program
        return KEEP_RUNNING;
    }
}