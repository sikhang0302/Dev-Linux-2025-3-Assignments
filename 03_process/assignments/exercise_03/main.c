#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h> 

// flag to check if signal is received in child process
int isSignalReceived = 0;

// function to handle SIGUSR1 from parent process for child process
void handle_sigusr1(int sig) {
    if (sig == SIGUSR1) {
        printf("Received signal SIGUSR1 from parent process!\n");
        isSignalReceived = 1;
    }
}

int main(int argc, char const *argv[]) {
    pid_t child_pid;

    child_pid = fork();
    
    if (child_pid == 0) { /* child process -> fork() returns 0 */
        // register the signal handler for SIGUSR1
        signal(SIGUSR1, handle_sigusr1);
        printf("I'm a child process\n");

        // infinite loop to wait for a signal from parent process
        while(!isSignalReceived);

        // exit the child process after handling the signal
        printf("Child process ended\n");
        exit(0); 
    } else if (child_pid > 0) { /* parent process -> fork() returns PID of its  child process */
        printf("I'm a parent process\n");

        // wait for 2 seconds
        sleep(2); 

        // send the SIGUSR1 signal to the child process
        printf("Parent process is sending signal SIGUSR1 to child process with PID: %d\n", child_pid);
        kill(child_pid, SIGUSR1);

        // wait for the child process to finish
        wait(NULL);
        printf("Parent process ended\n");
    } else { /* error -> fork() returns -1 */
        printf("fork() unsucessfully\n");
        exit(1);
    }

    return 0;
}