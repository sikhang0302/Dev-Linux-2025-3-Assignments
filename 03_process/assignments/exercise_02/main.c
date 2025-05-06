#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(void) {
    pid_t child_pid;

    child_pid = fork();

    if (child_pid == 0) { /* child process -> fork() returns 0 */
        char *action = getenv("ACTION"); // get environment variable named ACTION

        if (action == NULL || strlen(action) == 0) {
            printf("Enviroment variable ACTION does not exist\n");
            exit(1);
        }

        if (strcmp(action, "1") == 0) {
            // replace child process with program ls -l
            execlp("/usr/bin/ls", "ls", "-l", NULL); 
        } else if (strcmp(action, "2") == 0) {
            // replace child process with program date
            execlp("/usr/bin/date", "date", NULL);
        } else {
            printf("Invalid value of enviroment variable ACTION! (only accept 1 or 2)\n");
            exit(1);
        }

        // in case exec unsuccessfully
        perror("exec");
        exit(1);
    } else if (child_pid > 0) { /* parent process -> fork() returns PID of its child process */
        wait(NULL); // wait for child process to end
        printf("Parent process ended\n");
    } else { /* error -> fork() returns -1 */
        printf("fork() unsuccessfully\n");
        exit(1);
    }

    return 0;
}