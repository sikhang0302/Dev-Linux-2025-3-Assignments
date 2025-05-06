#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// define array size
#define SIZE 100

// global variables to store count results
int even_count = 0;
int odd_count = 0;

// function to generate random array
void generate_random_array(int arr[]) {
    for (int i = 0; i < SIZE; i++) {
        // generate a random number in the range 1 to 100
        arr[i] = rand() % 100 + 1;
    }
}

// handler function to count even numbers in the array
void *thread_count_even_handler (void *arg) {
    // cast the void pointer to an integer pointer
    int *arr = (int *) arg;
    for (int i = 0; i < SIZE; i++) {
        if (arr[i] % 2 == 0) { // if current number is even
            // increase the even counter
            even_count++;
        }
    }

    // exit the current thread
    pthread_exit(NULL);
} 

// handler function to count odd numbers in the array
void *thread_count_odd_handler (void *arg) {
    // cast the void pointer to an integer pointer
    int *arr = (int *) arg;
    for (int i = 0; i < SIZE; i++) { 
        if (arr[i] % 2 != 0) { // if current number is odd
            // increase the odd number
            odd_count++;
        }
    }

    // exit the current thread
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    // initialize random seed
    srand(time(NULL));

    // initialize a local array with 0
    int arr[SIZE] = { 0 }; 

    // assign random numbers between 1 and 100 to the local array
    generate_random_array(arr);

    pthread_t thread_count_even;
    pthread_t thread_count_odd;
    int return_code;

    // create the thread to count even numbers, pass the local array to the handler function, and check for error
    if (return_code = pthread_create(&thread_count_even, NULL, &thread_count_even_handler, (void *)arr)) {
        printf("pthread_create() error number=%d\n", return_code);
        return -1;
    }

    // create the thread to count odd numbers, pass the local array to the handler function, , and check for error
    if (return_code = pthread_create(&thread_count_odd, NULL, &thread_count_odd_handler, (void *)arr)) {
        printf("pthread_create() error number=%d\n", return_code);
        return -1;
    }

    // wait for the thread to count even numbers to finish and check for error
    if (return_code = pthread_join(thread_count_even, NULL)) {
        printf("pthread_join() error number=%d\n", return_code);
        return -1;
    }

    // wait for the thread to count even numbers to finish and check for error
    if (return_code = pthread_join(thread_count_odd, NULL)) {
        printf("pthread_join() error number=%d\n", return_code);
        return -1;
    }   

    // print the results
    printf("Number of even elements in the array: %d\n", even_count);
    printf("Number of odd elements in the array: %d\n", odd_count);

    return 0;
}