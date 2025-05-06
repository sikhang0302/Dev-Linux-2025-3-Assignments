#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc < 4 || argc > 5) {
        // Print the error message and exit
        write(STDERR_FILENO, "Usage: ./example_program <filename> <num-bytes> <r/w> [\"content\"]\n", 70);
        return 1;
    }

    char *filename = argv[1];
    int num_bytes = atoi(argv[2]);
    char mode = argv[3][0];
    char *content = argv[4];

    // Check for mode to open file (r or w)
    if (mode != 'r' && mode != 'w') {
        // Print the error message and exit
        write(STDERR_FILENO, "Invalid mode. Use 'r' for read or 'w' for write.\n", 50);
        return 1;
    }

    // If mode is r
    if (mode == 'r') {
        if (argc != 4) {
            // Print the error message and exit
            write(STDERR_FILENO, "Read mode does not need to take a content argument.\n", 52);
            return 1;
        } else {
            int fd = open(filename, O_RDONLY);
            if (fd == -1) {
                // Print the error message and exit
                perror("Error opening file for reading");
                return 1;
            }

            struct stat file_stat;
            // stat() returns -1 if file does not exist or can not access
            if (stat(filename, &file_stat) == -1) {
                // Print the error message and exit
                perror("Error getting file information");
                return 1;
            }
            // Get size of file
            int file_size = (int)file_stat.st_size;
            // Check number of bytes entered by user and the actual number of bytes in file before reading
            int bytes_to_read = num_bytes < file_size ? num_bytes : file_size;
            char *buffer = (char *)malloc(bytes_to_read);
            if (buffer == NULL) {
                // Print the error message and exit
                perror("Error allocating memory");
                close(fd);
                return 1;
            }

            // ssize_t is signed size_t
            ssize_t bytes_read = read(fd, buffer, bytes_to_read);
            if (bytes_read == -1) {
                // Print the error message and exit
                perror("Error reading file");
                free(buffer);
                close(fd);
                return 1;
            }

            // Add null terminator to the end of string
            buffer[bytes_read] = '\0';

            // Print the result and exit
            write(STDOUT_FILENO, buffer, bytes_read);
            free(buffer);
            close(fd);
        }
    }
    
    // If mode is w
    else if (mode == 'w') {
        if (argc != 5) {
            // Print the error message and exit
            write(STDERR_FILENO, "Write mode requires a content argument.\n", 40);
            return 1;
        } else {
            // Get the length of content needed to write
            int content_len = strlen(content);

            int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            if (fd == -1) {
                // Print the error message and exit
                perror("Error opening file for writing");
                return 1;
            }

            // Check number of bytes entered by user and the actual number of byte in content string before writing
            int bytes_to_write = num_bytes < content_len ? num_bytes : content_len;
            ssize_t bytes_written = write(fd, content, bytes_to_write);
            if (bytes_written == -1) {
                // Print the error message and exit
                perror("Error writing to file");
                close(fd);
                return 1;
            }

            // Print the result and exit
            write(STDOUT_FILENO, "Written to file: ", 17);
            write(STDOUT_FILENO, content, bytes_to_write);
            write(STDOUT_FILENO, "\n", 1);
            close(fd);
        }
    }
    else {
        // do nothing
    }

    return 0;
}