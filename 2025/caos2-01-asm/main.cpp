#include <stdio.h>

extern "C"
int multiply_add(int, int);

int main() {
    int B = 2000;
    int A = 100;

    int res = multiply_add(A, B);
    printf("Number: %d\n", res);
}
