#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 15000000000ULL

typedef struct {
    uint64_t l, r;
    uint64_t result;
} Args;

uint64_t compute(uint64_t l, uint64_t r) {
    uint64_t prod = 1;

    for (uint64_t i = l; i < r; ++i)
        prod *= (i | 1);

    return prod;
}

void* worker(void* arg) {
    Args* a = arg;
    a->result = compute(a->l, a->r);
    return NULL;
}

int main(int argc, char** argv) {
    int single_core =
        argc > 1 && strcmp(argv[1], "--single-core") == 0;

    pthread_t threads[2];
    Args args[2] = {
        {0, N / 2, 0},
        {N / 2, N, 0}
    };

    for (int i = 0; i < 2; ++i) {
        pthread_attr_t attr;
        pthread_attr_init(&attr);

        cpu_set_t cpus;
        CPU_ZERO(&cpus);
        CPU_SET(single_core ? 0 : i, &cpus);

        pthread_attr_setaffinity_np(
            &attr, sizeof(cpus), &cpus
        );

        int rc = pthread_create(
            &threads[i], &attr, worker, &args[i]
        );
        if (rc != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(rc));
            exit(1);
        }

        pthread_attr_destroy(&attr);
    }

    for (int i = 0; i < 2; ++i)
        pthread_join(threads[i], NULL);

    printf("Result: %llu\n",
           (unsigned long long)
           (args[0].result * args[1].result));

    return 0;
}
