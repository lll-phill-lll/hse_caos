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
    int* ids = calloc(N, sizeof(int));

    if (!threads || !ids) {
        free(threads);
        free(ids);
        return 1;
    }

    int id = 0;

    for (int i = 0; i < N; ++i) {
        ids[i] = id++;
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    }

    for (int i = 0; i < N; ++i) {
        pthread_join(threads[i], NULL);
    }

    free(ids);
    free(threads);
    return 0;
}
