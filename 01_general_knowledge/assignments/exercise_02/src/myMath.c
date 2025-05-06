#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double add(double a, double b) {
    return a + b;
}

double subtract(double a, double b) {
    return a - b;
}

double multiply(double a, double b) {
    return a * b;
}

double divide(double a, double b) {
    if (b == 0) {
        printf("Error: Division by zero!\n");
        return 0;
    }
    return a / b;
}

unsigned long long factorial(int n) {
    if (n < 0) {
        printf("Error: Factorial of negative number is undefined!\n");
        return 0;
    }
    unsigned long long result = 1;
    for (int i = 1; i <= n; i++) {
        result *= i; 
    }
    return result;
}

double power(double base, int exponent) {
    double result = 1.0;
    for (int i = 0; i < abs(exponent); i++) {
        result *= base;
    }
    if (exponent < 0) {
        return 1.0 / result;
    }
    return result;
}

int is_prime(int n) {
    if (n <= 1) {
        return 0;
    }
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return 0;
        }
    }
    return 1;
}