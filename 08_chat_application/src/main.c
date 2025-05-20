#include "../inc/chat_core.h"
#include "../inc/chat_cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
    int port_no;

    // Check if port number is provided
    if (argc < 2) {
        printf("No port provided\ncommand: ./chat <port number>\n");
        exit(EXIT_FAILURE);
    } else {
        port_no = atoi(argv[1]);
    }
    
    // Display welcome message and help
    printf("************************************ Chat Application ************************************\n\n");
    show_help();
    printf("******************************************************************************************\n\n");
    
    // Start the server and handle peer communication
    start_server(port_no);
    sleep_in_ms(2); // Small delay to ensure server thread starts first
    handle_peer_communication(port_no, process_command);
    cleanup_chat_system(); // Cleanup before exiting
    
    return 0;
}