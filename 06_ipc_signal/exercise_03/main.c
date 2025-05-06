#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_SIGNAL_COUNT 5
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int signal_count = 0;

void signal_handler(int num) {
    printf("Received signal from parent\n");
    signal_count++;
    if (signal_count == MAX_SIGNAL_COUNT) {
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char* argv[]) {
    pid_t child_pid;
    
    child_pid = fork();
    if (child_pid == 0) { // child process -> fork() returns 0
        signal(SIGUSR1, signal_handler);
        while(1);
    } else if (child_pid > 0) { // parent process -> fork() returns PID of its child process
        for (int i = 0; i < MAX_SIGNAL_COUNT; i++) {
            sleep(2);
            kill(child_pid, SIGUSR1);
        }
        wait(NULL);
    } else { // error -> fork() returns -1
        handle_error("fork()");
    }

    return 0;
}

/************************************************************************************************************************************************ 
Câu hỏi: Tại sao chúng ta cần sử dụng kill() để gửi tín hiệu trong bài này?
- kill() là hàm hệ thống dùng để gửi tín hiệu đến một tiến trình cụ thể dựa trên PID của tiến trình đó. Trong bài yêu cầu giao tiếp giữa 2 process 
  riêng biệt (process cha gửi tín hiệu cho process con), nên hàm này sẽ cho phép process cha gửi tín hiệu đến process con thông qua PID và con có
  thể bắt và xử lí tín hiệu đó thông qua hàm signal().
************************************************************************************************************************************************/