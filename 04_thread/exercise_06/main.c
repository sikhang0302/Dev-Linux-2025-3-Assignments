#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// define the size of the array
#define ARRAY_SIZE 10000000
// define number of threads to handle each part of the array
#define NUM_OF_THREADS 4

// mutex to protect total sum variable
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// array to hold 10000000 integers
int arr[ARRAY_SIZE];
// global variable to store the total sum
long long total_sum = 0;

// handler function to calculate sum of a portition of the array
void *calculate_sum_handler(void *arg) {
    // get index of the current thread which defines its portion in the array
    int thread_part = *((int *)arg);
    
    // calculate start index and end index for the portion of the current thread in the array
    int start = thread_part * (ARRAY_SIZE / NUM_OF_THREADS);
    int end = (thread_part + 1) * (ARRAY_SIZE / NUM_OF_THREADS);

    // local variable to store the sum of the portion of the current thread in the array
    // local variable in handler function is unique to each thread, so there is no need for a mutex
    long long partial_sum = 0;
    for (int i = start; i < end; i++) {
        // sum of the elements in this portion  
        partial_sum += arr[i];
    }

    // lock the mutex before modifying the shared resource
    pthread_mutex_lock(&mutex);
    // critical section: add the partial sum of the current thread to the total sum
    total_sum += partial_sum;
    // ulock the mutex after updating
    pthread_mutex_unlock(&mutex);

    // exit current thread
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    // initialize the array with values
    for (int i = 0; i < ARRAY_SIZE; i++) {
        // assume all array elements are set to 1
        arr[i] = 1;
    }

    // array to hold threads identifiers
    pthread_t threads[NUM_OF_THREADS];
    // array to hold the index of each thread
    int thread_indices[NUM_OF_THREADS];
    int return_code;

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        // set the index for the current thread
        thread_indices[i] = i;
        // create threads, assign each one a portion of the array to process, and check for errors 
        if (return_code = pthread_create(&threads[i], NULL, &calculate_sum_handler, (void *)(&thread_indices[i]))) {
            printf("pthread_create() error number=%d\n", return_code);
            return -1;
        }
    }

    // wait for all threads to finish and check for errors
    for (int i = 0; i < NUM_OF_THREADS; i++) {
        if (return_code = pthread_join(threads[i], NULL)) {
            printf("pthread_join() error number=%d\n", return_code);
            return -1;
        }
    }

    // print the result of the total sum
    printf("Total sum of the array: %lld\n", total_sum);

    // destroy the mutex to clean up
    pthread_mutex_destroy(&mutex);

    return 0;
}