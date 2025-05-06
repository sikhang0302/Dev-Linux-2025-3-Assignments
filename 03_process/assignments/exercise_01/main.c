#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
    pid_t child_pid;

    child_pid = fork();
    if (child_pid >= 0) {
        if (child_pid == 0) { /* Child process -> fork() returns 0 */
            printf("I'm the child process\n");
            /* Get PID of child process and print the result */
            printf("My PID is: %d\n", getpid()); 
        } else { /* Parent process -> fork() returns PID of its child process */
            /* Wait 0.05s to make sure child process execute first */
             usleep(50000);  // 50,000 microseconds = 50ms = 0.05s
            printf("\nI'm the parent process\n");
            /* Get PID of child process and print the result */
            printf("PID of my child process is: %d\n", child_pid);
        }
    } else {
        printf("fork() unsuccessfully\n"); /* error -> fork() returns -1 */
        exit(1);
    }

    return 0;
}