// C program for exercise 01
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include<fcntl.h>

int main(void) {
    int fd;
    int numb_write;
    char buf1[12] = "hello world\n";

    fd = open("hello.txt", O_RDWR | O_CREAT | O_APPEND, 0667);
    if (fd == -1) {
        printf("open() hello.txt failed\n");
    }

    numb_write = write(fd, buf1, strlen(buf1));
    printf("Write %d bytes to hello.txt\n", numb_write);

    lseek(fd, 2, SEEK_SET);
    write(fd, "AAAAAAAAAAAA", strlen("AAAAAAAAAAAA"));
    
    close(fd); 

    return 0;
}

/* 
Trả lời cho BT1:
- Dữ liệu sẽ xuất hiện ở vị trí cuối file vì nếu một file được mở với cờ O_APPEND, tức là nó đang ở chế độ append, lúc này trước 
mỗi thao tác ghi (write), offset của file luôn được định vị ở cuối file, bất kể con trỏ file hiện tại đang ở đâu. 
- Do đó nếu dùng lseek để thay đổi vị trí con trỏ file, nhưng khi ghi dữ liệu thì cờ O_APPEND sẽ bỏ qua vị trí đó và tự động nhảy 
đến cuối file để thực hiện ghi.
*/ 