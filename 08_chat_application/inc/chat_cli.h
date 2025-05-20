#ifndef CLI_H
#define CLI_H

#include "chat_core.h"

void show_help();                                            // Display help information for available commands
ProgramState_t process_command(const char *buffer,           // Process user commands and return the program state
    int listen_port, const char *server_ip);

#endif