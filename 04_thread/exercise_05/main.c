#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// define number of reader threads 
#define NUM_OF_READERS 5
// define number of writer threads 
#define NUM_OF_WRITERS 2

// global variable to store the data 
int data = 0;
// initialize the read-write lock 
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

// handler function for reader thread
void *reader_thread_handler (void *arg) {
    // get index of the current thread
    int id = *((int *)arg);
    
    // acquire read lock
    pthread_rwlock_rdlock(&rwlock);
    printf("Reader %d: read data = %d\n", id, data);
    // simulate reading delay by waiting for 1 second
    sleep(1);
    printf("Reader %d: done reading\n", id);
    // release read lock
    pthread_rwlock_unlock(&rwlock);

    // exit the current thread
    pthread_exit(NULL);
}

// handler function for writer thread
void *writer_thread_handler (void *arg) {
    // get index of the current thread
    int id = *((int *)arg);

    // acquire write lock
    pthread_rwlock_wrlock(&rwlock);
    // critical section: modify shared data
    printf("Writer %d: writing...\n", id);
    data += 1;
    // simulate writing delay by waiting for 2 seconds
    sleep(2);
    printf("Writer %d: done writing, data = %d\n", id, data);
    // release write lock
    pthread_rwlock_unlock(&rwlock);

    // exit the current thread
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    // array to hold reader threads
    pthread_t reader_threads[NUM_OF_READERS];
    // array to hold writer threads
    pthread_t writer_threads[NUM_OF_WRITERS];
    // array to hold the index of each reader thread
    int reader_ids[NUM_OF_READERS];
    // array to hold the index of each writer thread
    int writer_ids[NUM_OF_WRITERS];
    int return_code;

    for (int i = 0; i < NUM_OF_READERS; i++) {
        // set the index for the current thread
        reader_ids[i] = i + 1;
        // create reader thread and check for error
        if (return_code = pthread_create(&reader_threads[i], NULL, &reader_thread_handler, (void *)(&reader_ids[i]))) {
            printf("pthread_create() error number=%d\n", return_code);
            return -1;
        } 
    }

    for (int i = 0; i < NUM_OF_WRITERS; i++) {
        // set the index for the current thread
        writer_ids[i] = i + 1;
        // create writer thread and check for error
        if (return_code = pthread_create(&writer_threads[i], NULL, &writer_thread_handler, (void *)(&writer_ids[i]))) {
            printf("pthread_create() error number=%d\n", return_code);
            return -1;
        }
    }

    // wait for all reader threads to finish and check for errors
    for (int i = 0; i < NUM_OF_READERS; i++) {
        if (return_code = pthread_join(reader_threads[i], NULL)) {
            printf("pthread_join() error number=%d\n", return_code);
            return -1;
        }
    }

    // wait for all writer threads to finish and check for errors
    for (int i = 0; i < NUM_OF_WRITERS; i++) {
        if (return_code = pthread_join(writer_threads[i], NULL)) {
            printf("pthread_join() error number=%d\n", return_code);
            return -1;
        }
    }

    // print final value of data
    printf("Final value of data: %d\n", data);

    // destroy the read-write lock   to clean up
    pthread_rwlock_destroy(&rwlock);

    return 0;
}

/************************************************************************************************************************************************ 
1. pthread_rwlock_t - Read-Write Lock
- Là một loại khóa đặc biệt cho phép:
    + Nhiều thread có thể đọc đồng thời (miễn là không có thread ghi nào đang giữ khóa)
    + Chỉ một thread được phép ghi tại một thời điểm, và không có thread đọc nào được phép chạy cùng lúc đang ghi

2. pthread_rwlock_rdlock(&rwlock) - Khóa đọc
- Khi một thread gọi hàm này, nó xin quyền đọc dữ liệu
- Thread sẽ được cho phép đọc ngay nếu:
    + Không có thread nào đang giữ khóa ghi (wrlock)
    + Có thể có nhiều thread cùng giữ rdlock cùng một lúc (đọc song song)
- Nếu đang có thread ghi thì thread đọc sẽ phải chờ cho đến khi ghi xong và mở khóa
- Dùng khi thread chỉ cần đọc dữ liệu, không cần thay đổi nó

3. pthread_rwlock_wrlock(&rwlock) - Khóa ghi
- Thread sẽ đợi cho đến khi không còn ai đọc và cũng không có ai đang ghi
- Sau đó, nó sẽ được trao quyền độc quyền ghi dữ liệu
- Trong lúc thread giữ wrlock, không thread nào khác có thể đọc hoặc ghi
- Dùng khi thread cần ghi/thay đổi dữ liệu được bảo vệ  

4. Quy tắc của pthread_rwlock:
- Nhiều thread đọc có thể chạy cùng lúc, miễn là không có thread ghi nào đang giữ khóa
- Chỉ một thread ghi được phép chạy tại một thời điểm, và khi đó không có thread đọc hoặc ghi nào khác được phép chạy
************************************************************************************************************************************************/