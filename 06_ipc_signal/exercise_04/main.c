#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handle_sigtstp(int sig) {
    printf("\nSIGTSTP ignored\n");
}

int main() {
    // Method 1: Use signal()
    signal(SIGTSTP, handle_sigtstp);
    
    // Method 2: Use sigaction()
    /*
    struct sigaction sa;
    sa.sa_handler = handle_sigtstp;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTSTP, &sa, NULL);
    */
    
    printf("Program is running. Press Ctrl+Z to test SIGTSTP.\n");
    
    while (1) {
        printf("Program is keep running...\n");
        sleep(1);
    }
    
    return 0;
}
/************************************************************************************************************************************************
Lưu ý 1: Trên một số hệ thống, tín hiệu SIGTSTP có thể không thể bị bắt hoặc bỏ qua hoàn toàn
Lưu ý 2: Tín hiệu SIGTSTP có hai lớp xử lí độc lập
1. Lớp tiến trình:
- Có thể bắt/bỏ qua tín hiệu SIGTSTP bằng signal() hoặc sigaction() để ngăn tiến trình bị dừng khi nhận tín hiệu SIGTSTP (khi nhấn Ctrl+Z)
- Nhưng điều này không làm ảnh hưởng đến lớp shell
2. Lớp shell:
- Khi nhấn Ctrl+Z, shell nhận sự kiện từ terminal và tự động đưa tiến trình từ foreground xuống background (dù tiến trình có bị dừng hay không)
- Đây là hành vi mặc định của shell, không phụ thuộc vào cách tiến trình xử lí tín hiệu SIGTSTP
Do đó, trong bài này, dù tín hiệu SIGTSTP có bị bắt và xử lí theo cách khác của tiến trình thì shell vẫn đẩy tiến trình xuống background và tiến
trình vẫn tiếp tục chạy dưới đó
************************************************************************************************************************************************/

/************************************************************************************************************************************************ 
Câu hỏi: Điều gì xảy ra nếu không xử lý tín hiệu SIGTSTP và người dùng nhấn Ctrl+Z?
- Theo mặc định của tín hiệu SIGTSTP khi nhấn Ctrl+Z, tiến trình sẽ bị tạm dừng (suspend) và được shell chuyển vào trạng thái nền (background)
- Shell sẽ hiển thị thông báo "[1] + Stopped" để chỉ ra tiến trình đã bị dừng
- Tiến trình sẽ không tiếp tục được thực thi cho đến khi được đưa trở lại foreground (bằng lệnh fg) hoặc được tiếp tục bằng tín hiệu SIGCONT
************************************************************************************************************************************************/