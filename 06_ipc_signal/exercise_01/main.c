#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int sigint_count = 0;

void signal_handler(int num) {
    printf("\nSIGINT received\n");
    sigint_count++;

    if (sigint_count == 3) {
        printf("Received SIGINT 3 times. Exit the program...\n");
        exit(EXIT_SUCCESS);
    }
}


int main (int argc, char* argv[]) {
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
         fprintf(stderr, "Couldn't handle SIGINT\n");
         exit(EXIT_FAILURE);
    }

    // if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
    //     fprintf(stderr, "Couldn't ignore SIGINT\n");
    //     exit(EXIT_FAILURE);
    // }

    while(1) {
        printf("The program is running...\n");
        sleep(2);
    }

    return 0;
}

/************************************************************************************************************************************************ 
Câu hỏi: Nếu bỏ qua tín hiệu SIGINT, chuyện gì sẽ xảy ra khi nhấn Ctrl+C?
- Nếu chương trình bỏ qua (ignore) tín hiệu SIGINT (ví dụ khi dùng signal(SIGINT, SIG_IGN);) thì khi nhấn Ctrl+C, chương trình sẽ không bị ảnh
  hưởng bởi tín hiệu SIGINT và vẫn tiếp tục chạy bình thường.
- Lúc này dù tín hiệu được gửi đi, nhưng chương trình không thực hiện hành động gì và không bị dừng.
************************************************************************************************************************************************/