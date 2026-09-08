#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define N 1000000
#define REPS 200

double now() {
    return (double)clock() / CLOCKS_PER_SEC;
}

int main() {
    uint32_t *a = malloc(N * sizeof(uint32_t));
    if (!a) return 1;

    for (int i = 0; i < N; i++)
        a[i] = rand();

    int evens;
    double t1, t2;

    t1 = now();
    for (int r = 0; r < REPS; r++) {
        evens = 0;
        for (int i = 0; i < N; i++)
            if ((a[i] & 1) == 0)
                evens++;
    }
    t2 = now();

    printf("Random order: %f sec\n", t2 - t1);

    int left = 0, right = N - 1;
    while (left < right) {
        while (left < N && (a[left] & 1) == 0) left++;
        while (right > 0 && (a[right] & 1) == 1) right--;
        if (left < right) {
            uint32_t tmp = a[left];
            a[left] = a[right];
            a[right] = tmp;
        }
    }

    t1 = now();
    for (int r = 0; r < REPS; r++) {
        evens = 0;
        for (int i = 0; i < N; i++)
            if ((a[i] & 1) == 0)
                evens++;
    }
    t2 = now();

    printf("Parity grouped: %f sec\n", t2 - t1);

    free(a);
    return 0;
}
