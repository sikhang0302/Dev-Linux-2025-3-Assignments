#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <libgen.h> 

int main(void) {
    int fd;
    int numb_write;
    char buf1[12] = "Hello world\n";
    const char *file = "exercise_04.txt";

    fd = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        // Print the error message and exit
        perror("Error opening file");
        return 1;
    }

    numb_write = write(fd, buf1, strlen(buf1));
    if (numb_write == -1) {
        // Print the error message and exit
        perror("Error writing to file");
        close(fd);
        return 1;
    } else {
        printf("- Write %d bytes to exercise_04.txt\n", numb_write);
    }
    
    // Bring fd to the start of the file to read
    lseek(fd, 0, SEEK_SET);
    printf("- File content:\n");
    char *buffer = (char *)malloc(numb_write);
    if (buffer == NULL) {
        // Print the error message and exit
        perror("Error allocating memory");
        close(fd);
        return 1;
    }
    // ssize_t is signed size_t
    ssize_t bytes_read = read(fd, buffer, numb_write);
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

    struct stat file_stat;
    // stat() returns -1 if file does not exist or can not access
    if (stat(file, &file_stat) == -1) {
        // Print the error message and exit
        perror("Error getting file information");
        return 1;
    }

    // Get type of file
    printf("- File Type:\n");
    switch(file_stat.st_mode & S_IFMT) {
        case S_IFBLK:
        {
            printf("Block Device\n");
            break;
        }
        case S_IFCHR:
        {
            printf("Character Device\n");
            break;
        }
        case S_IFIFO:
        {
            printf(" FIFO/Pipe\n");
            break;
        }
        case S_IFREG:
        {
            printf("Regular File\n");
            break;
        }
        case S_IFDIR:
        {
            printf("Directory\n");
            break;
        }
        case S_IFLNK:
        {
            printf("Symlink\n");
            break;
        }
        case S_IFSOCK:
        {
            printf("Socket\n");
            break;
        }
        default:
        {
            printf("Unknow type of file\n");
            break;
        }
    }

    // Get file name
    char *file_copy = malloc(strlen(file) + 1); // add 1 for '\0' character
    if (file_copy == NULL) {
         // Print the error message and exit
        perror("Error allocating memory");
        close(fd);
        return 1;
    }
    strcpy(file_copy, file);
    char *file_name = basename(file_copy);
    printf("- File name:\n%s\n", file_name);
    free(file_copy);

    // Get file serial number
    printf("- File serial number (inode):\n%ju\n", (uintmax_t)file_stat.st_ino);

    // Get time of last data modification
    printf("- Last data modification on file:\n%s", (ctime)(&file_stat.st_mtime));

    // Get size of file
    printf("- File size:\n%jd bytes\n",(intmax_t)file_stat.st_size);

    close(fd);

    return 0;
}