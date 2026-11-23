#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* worker(void* arg) {
    int id = *(int*)arg;
    printf("Thread %d\n", id);
    return NULL;
}

int main() {
    int N = 10;
    pthread_t* threads = calloc(N, sizeof(pthread_t));

    int id = 0;

    for (int i = 0; i < N; ++i) {
        pthread_create(&threads[i], NULL, worker, &id);
        id++;
    }

    for (int i = 0; i < N; ++i) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    return 0;
}
