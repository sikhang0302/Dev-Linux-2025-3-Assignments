#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h> 
#include <ctype.h>

// function to check if string is number
int isNumber(const char *str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }

    for (int i = 0; str[i] != '\0'; i++) {
        if(!isdigit(str[i])) {
            return 0;
        }
    }
    
    return 1;
}

int main(int argc, char const *argv[]) {
    pid_t child_pid;

    child_pid = fork();

    if (child_pid == 0) {  /* child process -> fork() returns 0 */
        printf("I'm a child process with PID: %d\n", getpid());

        const char *exitCodeEnv = getenv("EXIT_CODE");
        int exitCode = 0;

        if (exitCodeEnv == NULL) {
            printf("EXIT_CODE is not set!\n");
            exit(1);
        } else if (isNumber(exitCodeEnv) == 0) {
            printf("Invalid EXIT_CODE!\n");
            exit(1);
        }

        exitCode = atoi(exitCodeEnv);

        // Check if value of EXIT_CODE is from 0 to 255 
        if (exitCode < 0 || exitCode > 255) {
            printf("EXIT_CODE must be between 0 and 255!\n");
            exit(1);
        }

        printf("Child process will be terminated with exit code: %d after 2 seconds\n", exitCode);
        // wait for 2 seconds
        sleep(2);
        // exit the child process with value of exitCode
        printf("Child process ended\n");
        exit(exitCode);
    } else if (child_pid > 0) { /* parent process -> fork() returns PID of its  child process */
        printf("I'm a parent process, my child PID is: %d\n", child_pid);
        int status;
        int returnValueOfWait;
        // wait for the child process to finish and get its exit status
        wait(&status);
        if (returnValueOfWait == -1) { /* error -> wait() returns -1 */
            printf("wait() unsucessfully\n");
            exit(1);
        } 

        if(WIFEXITED(status)) {
            printf("Child process is normally terminated with exit code: %d\n", WEXITSTATUS(status));
        } else if(WIFSIGNALED(status)) {
            printf("Child process is abnormally terminated with status: %d\n", WTERMSIG(status));
        }

        printf("Parent process ended\n");
    } else { /* error -> fork() returns -1 */
        printf("fork() unsucessfully\n");
        exit(1);
    }


    return 0;
}