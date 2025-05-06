#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

// declare global variables to store thread IDs
pthread_t tid1;
pthread_t tid2;

// thread handler function
static void *thread_handler(void* thread_id) {
    // get ID of current thread
    pthread_t tid_to_check = pthread_self();

    // compare current thread ID with tid1 to determine which thread is running
    if (pthread_equal(tid_to_check, tid1)) {
        // thread 1 is running
        printf("Thread 1: Hello with thread id %llu\n", (unsigned long long)tid_to_check);
    } else {
        // thread 2 is running
        printf("Thread 2: Hello with thread id %llu\n", (unsigned long long)tid_to_check);
    }
    
    // exit the current thread with no return value
    pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
    // get ID of main thread
    pthread_t main_tid = pthread_self();
    int return_code;

    // create thread 1 and check for error
    if (return_code = pthread_create(&tid1, NULL, &thread_handler, NULL)) {
        printf("pthread_create() error number=%d\n", return_code);
        return -1;
    }

    // wait for 2 seconds
    sleep(2);

    // create thread 2 and check for error
    if (return_code = pthread_create(&tid2, NULL, &thread_handler, NULL)) {
        printf("pthread_create() error number=%d\n", return_code);
        return -1;
    }

    // wait for 5 seconds
    sleep(5);

    printf("Main Thread: Hello with thread id %llu\n", (unsigned long long)main_tid);

    // wait for thread 1 to finish and check for error
    return_code = pthread_join(tid1, NULL);
    if (return_code) {
        printf("pthread_join() error number =%d\n", return_code);
        return -1;
    } else {
        printf("Main Thread: Thread 1 has finished!\n");
    }
    // wait for thread 2 to finish and check for error
    return_code = pthread_join(tid2, NULL);
    if (return_code) {
        printf("pthread_join() error number =%d\n", return_code);
        return -1;
    } else {
        printf("Main Thread: Thread 2 has finshed!\n");
    }

    printf("Main thread: All threads have finished.\n");

    return 0;
}

/************************************************************************************************************************************************ 
Sử dụng pthread_create và pthread_join như thế nào? Khi nào thread kết thúc?
1. pthread_create:
- Dùng để tạo một thread mới
- Nguyên mẫu hàm: int pthread_create(pthread_t *threadID, const pthread_attr_t *attr, void *(*start) (void *), void *arg);
- Trong bài tập trên:
    + &tid1 và &tid2: Là tham chiếu đến biến lưu ID của thread 1 và thread 2
    + &thread_handler: là tham chiếu đến hàm handler của thread 1 và thread 2
- Giá trị trả về:
    + 0 nếu tạo thread thành công
    + Khác 0 nếu xảy ra lỗi khi tạo thread, lúc này thread sẽ không được tạo

2. pthread_join:
- Dùng để đợi một thread kết thúc
- Nguyên mẫu hàm: int pthread_join(pthread_t thread, void **retval);
- Nếu không dùng pthread_join để đợi các thread con kết thúc, main thread có thể kết thúc trước khi các thread con kịp hoàn thành
- Giá trị trả về:
    + 0 nếu join thành công
    + Khác 0 nếu xảy ra lỗi trong quá trình join

3. Thread kết thúc khi:
- Hàm của thread kết thúc (return)
- Gọi hàm pthread_exit()
************************************************************************************************************************************************/