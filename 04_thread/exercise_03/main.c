#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_OF_ITERATIONS 10

// initialize mutex and condition variable
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// global variable to store data created by the producer
int data = 0;
// flag to indicate if new data is ready
int data_ready = 0; 

// handler function of producer
void *producer_handler(void *arg) {
    for (int i = 0; i < NUM_OF_ITERATIONS; i++) {
        // generate a random integer number between 1 and 10
        int random_value = rand() % 10 + 1;

        // lock the mutex before accessing shared data
        pthread_mutex_lock(&mutex);

        // critical section: create new data
        data = random_value;
        // set the flag to indicate the data is ready
        data_ready = 1;

        // signal the consumer that new data is available
        pthread_cond_signal(&cond);

        // unlock the mutex
        pthread_mutex_unlock(&mutex);

        // wait for 1 seconds between productions
        sleep(1);
    }

    // exit the current thread
    pthread_exit(NULL);
}

// handler function of consumer
void *consumer_handler(void *arg) {
    for (int i = 0; i < NUM_OF_ITERATIONS; i++) {
        // lock the mutex before accessing shared data
        pthread_mutex_lock(&mutex);
        
        // wait until data is ready 
        while (data_ready == 0) {
            pthread_cond_wait(&cond, &mutex);
        }

        // read and print the data
        printf("Data read by consumer: %d\n", data);

        // reset the flag to indicate the data has been consumed
        data_ready = 0;

        // unlock the mutex
        pthread_mutex_unlock(&mutex);
    }

    // exit the current thread
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    // initialize random seed
    srand(time(NULL));

    pthread_t producer_thread;
    pthread_t consumer_thread;
    int return_code;

    // create producer thread and check for error
    if (return_code = pthread_create(&producer_thread, NULL, &producer_handler, NULL)) {
        printf("pthread_create() error number=%d\n", return_code);
        return -1;
    }

    // create consumer thread and check for error
    if (return_code = pthread_create(&consumer_thread, NULL, &consumer_handler, NULL)) {
        printf("pthread_create() error number=%d\n", return_code);
        return -1;
    }   

    // wait for producer thread to finish and check for error
    if (return_code = pthread_join(producer_thread, NULL)) {
        printf("pthread_join() error number=%d\n", return_code);
        return -1;
    }

    // wait for consumer thread to finish and check for error
    if (return_code = pthread_join(consumer_thread, NULL)) {
        printf("pthread_join() error number=%d\n", return_code);
        return -1;
    }

    // destroy the mutex to clean up
    pthread_mutex_destroy(&mutex);

    // destroy the condition variable to clean up
    pthread_cond_destroy(&cond);

    return 0;
}