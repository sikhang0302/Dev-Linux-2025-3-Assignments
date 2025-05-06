#include <stdio.h>
#include "../inc/myMath.h"

int main(void) {
    printf("Addition: 5 + 3 = %.2f\n", add(5, 3));
    printf("Subtraction: 5 - 3 = %.2f\n", subtract(5, 3));
    printf("Multiplication: 5 * 3 = %.2f\n", multiply(5, 3));
    printf("Division: 5 / 3 = %.2f\n", divide(5, 3));
    printf("Factorial: 5! = %llu\n", factorial(5));
    printf("Power: 2^3 = %.2f\n", power(5, 3));
    printf("Is 7 prime? %s\n", is_prime(7) ? "Yes" : "No");

    return 0;
}