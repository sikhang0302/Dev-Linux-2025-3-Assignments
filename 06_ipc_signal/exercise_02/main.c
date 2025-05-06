#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int timer_count = 0;

void signal_handler(int num) {
    timer_count++;
    if(timer_count == 1) {
        printf("Timer: %d second\n", timer_count);
    } else {
        printf("Timer: %d seconds\n", timer_count);
    }

    if(timer_count == 10) {
        printf("10 seconds have passed. Exit the program...\n");
        exit(EXIT_SUCCESS);
    }

    alarm(1);
}

int main(int argc, char* argv[]) {
    if (signal(SIGALRM, signal_handler) == SIG_ERR) {
        fprintf(stderr, "Couldn't handle SIGALRM\n");
        exit(EXIT_FAILURE);
    }
    alarm(1);

    while(1) {
        // do nothing;
    }

    return 0;
}

/************************************************************************************************************************************************ 
Câu hỏi: Điều gì xảy ra nếu không gọi lại alarm(1) trong hàm xử lý?
- Nếu không gọi lại alarm(1) trong hàm signal_handler(), thì:
    + Tín hiệu SIGALRM chỉ được gửi một lần duy nhất sau khi alarm(1) được gọi trong hàm main().
    + Lúc này chương trình chỉ tăng biến đếm timer_count lên đúng một lần, in ra "Timer: 1 second" rồi đứng yên mãi mãi (do vòng lặp while(1)
      trong hàm main()) và không tiếp tục đếm nữa.
    + Bởi vì alarm(1) chỉ có tác dụng phát tín hiệu SIGALRM sau một giây sau khi nó được gọi chứ không tự động lặp lại. Phải tự gọi lại alarm(1)
      trong hàm handler của SIGALRM sau mỗi lần nhận được tín hiệu này nếu muốn nó được lặp.
************************************************************************************************************************************************/