#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h> 

int main(int argc, char const *argv[]) {
    pid_t child_pid1;
    pid_t child_pid2;

    // tạo tiến trình con 1 trong tiến trình cha
    child_pid1 = fork();

    if (child_pid1 == 0) { // tiến trình con 1 - tạo Zombie
        printf("Child  process 1 (Zombie) - PID: %d\n", getpid());
        printf("Child process 1 ended\n");
        exit(0);  // con 1 kết thúc ngay lập tức -> trở thành zombie nếu cha không wait
    } else if (child_pid1 > 0) { // tiến trình cha
        printf("Parent process (PID: %d)\n", getpid());

        // tạo tiến trình con 2 trong tiến trình cha
        child_pid2 = fork();

        if (child_pid2 == 0) { // tiến trình con 2 - tạo Orphan
            // đợi 20 giây để đảm bảo tiến trình con 2 sống lâu hơn tiến trình cha để tạo ra orphan
            sleep(20);
            printf("Child process 2 (Orphan) - PID: %d, Parent PID: %d\n", getpid(), getppid());
            printf("Child process 2 ended\n");
            exit(0);    
        } else if (child_pid2 > 0) { // tiến trình cha
            printf("Parent process after second fork (PID: %d)\n", getpid());
        } else { // error -> fork() trả về -1
            printf("fork() unsucessfully\n");
            exit(1);
        }
        // đợi 10 giây để đảm bảo tiến trình cha sống lâu hơn tiến trình con 1 để tạo zombie
        // và kết thúc sớmh hơn tiến trình con 2 để tạo ra orphan
        sleep(10);
        printf("Parent process ended\n");
    } else { // error -> fork() trả về -1
        printf("fork() unsucessfully\n");
        exit(1);
    } 

    return 0;
}

/************************************************************************************************************************************************ 
Giải thích trạng thái zombie và orphan, cùng lý do tại sao chúng xuất hiện trong Linux
1. Trạng thái Zombie Process:
- Zombie Process là một tiến trình đã kết thúc (exit) nhưng vẫn còn tồn tại trong process table
- Chỉ thực sự biến mất khi tiến trình cha gọi wait() hoặc waitpid() để đọc exit status của nó
- Trong Linux, khi một tiến trình con kết thúc, kernel lưu lại thông tin trạng thái kết thúc của nó lại để tiến trình cha có thể xử lý
- Nếu tiến trình cha không gọi wait() hoặc waitpid(), kernel không thể xóa hẳn tiến trình con khỏi process table
- Nên nó sẽ trở thành Zombie Process (hay Defunct Process)
- Trong lệnh ps aux hoặc top, Zombie Process thường có:
    + STATUS: Z hoặc Z+
    + CMD: có chữ defunct
- Nếu có quá nhiều Zombie Process, chúng sẽ chiếm entry trong process table (có giới hạn), có thể khiến hệ thống không tạo được tiến trình mới

2. Trạng thái Orphan Process:
- Orphan Process là một tiến trình con vẫn đang chạy trong khi tiến trình cha của nó đã kết thúc (exit)
- Khi tiến trình cha kết thúc, tiến trình con được tiến trình ông nội (cha của tiến trình cha) hoặc init (PID 1) nhận làm cha mới
- Trong Linux, khi một tiến trình cha kết thúc trước khi tiến trình con của nó kết thúc, hệ điều hành cần một cha mới để quản lí tiến trình đó
- Tiến trình ông nội, init hoặc systemd sẽ nhận tiến trình này làm con để đảm bảo nó không thành Zombie Process sau này
- Lúc này tiến trình cha mới sẽ tự động wait() hoặc waitpid() cho Orphan Process khi nó kết thúc, nên không tạo Zombie Process 
************************************************************************************************************************************************/ 
