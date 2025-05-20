# Peer-to-Peer Chat Application

![Chat Application](https://img.shields.io/badge/Language-C-blue)
![Platform](https://img.shields.io/badge/Platform-Linux-lightgrey)

A simple chat application for message exchange among remote peers implemented in C

## Table of Contents
- [Features](#features)
- [Architecture](#architecture)
- [Makefile Key Notes](#makefile-key-notes)
- [Installation](#installation)
- [Usage Guide](#usage-guide)
- [Technical Notes](#technical-notes)

## Features

### Core Features
- **Peer-to-Peer Connection**: Connect to other peers using IP and port
- **Multi-peer Communication**: Connect with up to 10 peers simultaneously
- **Full-Duplex Chat**: Send and receive messages to connected peers in real-time
- **Command Interface**: Simple text-based command system
- **Connection Management**: List, connect, and terminate connections
- **Platform**: Works on Linux systems

### Technical Highlights
- **Dual-Role Design**: Acts as both server (for incoming connections) and client (for outgoing connections) simultaneously
- **Multi-threaded Design**: The server thread listens for incoming connections, while the main thread handles outgoing connections and user input
- **Thread Safety**: The code uses `pthread_mutex` to protect shared data (e.g., `peers` array)
- **Socket Programming**: TCP/IPv4 sockets with basic error handling is implemented for socket operations.
- **Peer Management**: Functions like `connect_to_peer`, `terminate_connection`, and `print_peers` manage peer connections
- **Command Processing**: The `process_command` function in `chat_cli.c` handles user input and delegates actions

## Architecture
The application uses a client-server model where each instance:
- Runs a server thread to accept incoming connections
- Maintains a list of connected peers
- Handles user input and message forwarding

### Key Components
- `chat_core.[ch]`: Core networking and peer management
- `chat_cli.[ch]:` Command line interface processing
- `main.c`: Application entry point
- `Makefile`: Build system

## Makefile Key Notes
- **Directory Structure**: Clearly defines where source, include, and binary files are located.
- **Compiler Flags**:
- `-Wall -Wextra`: enables comprehensive warnings
- `-I$(INC_DIR)`: adds include directory to header search path
- `-pthread`: enables POSIX threads support
- **Targets**:
- `all`: Default build target
- `install`: Copies the executable to the root directory
- `quick`: Combination of build and install
- `clean`: Removes all build artifacts
- **Automatic Source Detection**: Uses `wildcard` to automatically find all `.c` files in the source directory.

## Installation

###  Prerequisites
- GCC compiler
- GNU Make
- Linux/Unix operating system (tested on Ubuntu)
- POSIX thread libraries

### Build Instructions

**1. Build the application**:
```bash
make all
```
**2. Install to the current directory**:
```bash
make install
```
**3. (Optional) Quick build**
```bash
make quick
```

## Usage Guide
Run the chat application with a listening port:
``` bash
./chat <port_number>
```

### Available Commands
|     **Command**       |         **Description**        |        **Example**          |
|:---------------------:|:------------------------------:|:---------------------------:|
| `help`                | Show help menu                 | `help`                      |
| `myip`                | Display your IP address        | `myip`                      |
| `myport`              | Display your listening port    | `myport`                    |
| `connect <ip> <port>` | Connect to another peer        | `connect 192.168.1.10 8080` |
| `list`                | List all connections           | `list`                      |
| `terminate <id>`      | Terminate a connection         | `terminate 2`               |
| `send <id> <message>` | Send a message                 | `send 1 Hello world!`       |
| `exit`                | Close all connections and quit | `exit`                      |

### Runtime Example
```bash
# Terminal 1 (User A)
./chat 8080

# Terminal 2 (User B)
./chat 8081
> connect 127.0.0.1 8080
> send 0 Hello from User B!
```

## Technical Notes

### Limitations
- Fixed-sized peer array
- Fixed-sized buffer for user input

### Message Handling
- Max length: 100 characters (auto-truncated)
- Plain text format
- Immediate delivery

### Connection Limits
- Maximum 10 peers (configurable in `chat_core.h`)
- No NAT traversal (local network recommended)
- IPv4 only

### Common Issues
- `Bind failed`: Port already in use
- `Connection refused`: Peer not reachable
- `Invalid peer ID`: Check ID with `list` command