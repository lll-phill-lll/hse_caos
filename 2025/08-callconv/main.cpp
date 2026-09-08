#include <stdio.h>

int multiply_add(int, int);

extern int A;

int main() {
    int B = 2000;

    int res = multiply_add(A, B);
    printf("Number: %d\n", res);
}
