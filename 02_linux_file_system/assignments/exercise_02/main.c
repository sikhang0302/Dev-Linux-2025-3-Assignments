/*
Trả lời cho BT2:
1. fd1 = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    --> Mở file "file" với các cờ O_RDWR (mở file để đọc và ghi), O_CREAT (nếu file không tồn tại thì tạo mới), O_TRUNC (nếu file
        đã tồn tại, cắt độ dài của file về 0 hay xóa toàn bộ nội dung đang có trong file)
    --> Quyền truy cập được cấp là đọc và ghi và chỉ dành cho user (còn group và other thì không)
        --> Sử dụng S_IRUSR | S_IWUSR tương đương với 0600
    --> File này được gắn với con trỏ file fd1

2. fd3 = open(file, O_RDWR);
    --> Vẫn mở file "file" với cờ O_RDWR (mở file để đọc và ghi)
    --> File này được gắn thêm với con trỏ file fd3

3. write(fd1, "Hello,", 6);
    --> Ghi chuỗi "Hello," (tổng cộng 6 ký tự) vào file thông qua con trỏ file fd1
    --> Với fd1, file được mở với cờ O_TRUNC, nên nội dung của file sẽ bị xóa toàn bộ trước khi ghi. Do đó, lúc này file chỉ có "Hello," là
        nội dung đầu tiên hay nói cách khác nội dung của file hiện tại là "Hello,"

4. write(fd2, "world", 6);
    --> Gây ra lỗi vì con trỏ file fd2 chưa được khai báo hoặc mở trước đó. Nên không có tác động gì đến file "file"

5. lseek(fd2, 0, SEEK_SET);
    --> Gây ra lỗi vì con trỏ file fd2 chưa được khai báo hoặc mở trước đó. Nên không có tác động gì đến file "file"
    
6. write(fd1, "HELLO,", 6);
    --> Ghi chuỗi "HELLO," (tổng cộng 6 ký tự) vào file thông qua con trỏ file fd1
    --> Với fd1, file được mở với cờ O_TRUNC, nên nội dung của file trước đó sẽ bị xóa toàn bộ trước khi ghi. Do đó, "HELLO," sẽ thay thế cho
        nội dung trước đó ở lệnh 3 là "Hello," và lúc này nội dung của file là "HELLO,"

7. write(fd3, "Gidday", 6);
    --> Ghi chuỗi "Gidday" (tổng cộng 6 ký tự) vào file thông qua con trỏ file fd3
    --> Với fd3, file được mở với cờ O_RDWR (mở file để đọc và ghi) và do trước đó lseek cho f3 chưa được gọi nên khi gọi write sẽ ghi chuỗi
        "Gidday" vào vị trí hiện tại trong file, sau chuỗi "HELLO," đã được ghi ở lệnh 6
    --> Nên nội dung cuối cùng trong file sẽ là "HELLO,Gidday"
*/