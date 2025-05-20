# Phony targets (make will not check these as actual file names)
.PHONY = all clean install quick

# Directory definitions
CUR_DIR := .
BIN_DIR := $(CUR_DIR)/bin
INC_DIR := $(CUR_DIR)/inc
SRC_DIR := $(CUR_DIR)/src

# Compiler and flags
# Using gcc compiler
CC = gcc
# Compiler flags: warnings, include path, pthread
CFLAGS  = -Wall -Wextra -I$(INC_DIR) -pthread
 # Linker flags: pthread library
LDFLAGS = -pthread

# Target executable and source files
# Output executable path
TARGET = $(BIN_DIR)/chat
# Find all .c files in source directory
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Default target - builds the main executable
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(SOURCES)
# Create bin directory if it doesn't exist
	@mkdir -p $(BIN_DIR)
# Compile all sources into executable 
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Install target - copies the executable to current directory
install:
	cp $(TARGET) $(CUR_DIR)/chat

# Quick target - Same as install but ensures build first
quick: $(TARGET)
	cp $(TARGET) $(CUR_DIR)/chat

# Clean target - removes generated files
clean:
# Remove bin directory and installed executable
	rm -rf $(BIN_DIR) $(CUR_DIR)/chat