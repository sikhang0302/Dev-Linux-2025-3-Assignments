#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define NUM_OF_THREADS 3
#define INCREMENTS 1000000

// Shared counter variable
long long counter = 0;
// initialize mutex to protect the critical section
pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;

// thread function handler to increment the counter
void *increment_counter_thread_handler(void* arg) {
    for (int i = 0; i < INCREMENTS; i++) {
        // lock the mutex before accessing shared counter
        pthread_mutex_lock(&mutex_lock);
        // critical section: incrementing shared counter
        counter++;
        // unlock the mutex after updating
        pthread_mutex_unlock(&mutex_lock);
    }

    // exit current thread
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    pthread_t threads[NUM_OF_THREADS];
    int return_code;

    // create multiple threads and check for error
    for (int i = 0; i < NUM_OF_THREADS; i++) {
        if (return_code = pthread_create(&threads[i], NULL, &increment_counter_thread_handler, NULL)) {
            printf("pthread_create() error number=%d\n", return_code);
            return -1;
        }
    }

    // wait for all threads to finish and check for error
    for (int i = 0; i < NUM_OF_THREADS; i++) {
        if (return_code = pthread_join(threads[i], NULL)) {
            printf("pthread_join() error number=%d\n", return_code);
            return -1;
        }
    }

    // destroy the mutex to clean up
    pthread_mutex_destroy(&mutex_lock);

    // print the final value of the shared counter
    printf("Final value of counter is: %lld\n", counter);

    return 0;
}

/************************************************************************************************************************************************ 
Tại sao cần mutex trong bài này? Điều gì xảy ra nếu bỏ mutex?
1. Cần phải dùng mutex trong bài này vì:
- Khi có nhiều thread (cụ thể trong bài là 3) cùng truy cập và ghi vào biến counter mà không có đồng bộ, race condition có thể xảy ra
- Ví dụ: Hai thread cùng truy cập vào một giá trị biến counter = 5, mỗi thread cộng 1 vào biến này và đều ghi lại thành 6 (thay vì 7)

2. Nếu bỏ mutex thì:
- Kết quả cuối cùng của biến counter có thể không được như mong đợi, cụ thể là sẽ nhỏ hơn 3.000.000
- Lý do là các phép tăng không được thực hiện một cách atomic (chỉ có một thread được truy cập vào shared resource vào một thời điểm)
- Mà thay vào đó chúng lại được thực hiện một các non-atomic (nhiều thread cùng truy cập vào shared resource vào cùng một thời điểm)
- Điều này làm cho một số lần tăng bị mất đi do đè nhau
************************************************************************************************************************************************/