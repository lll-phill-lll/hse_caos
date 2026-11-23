#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void* worker(void* arg) {
    int id = *(int*)arg;

    for (int i = 0; i < 5; ++i) {
        printf("Worker %d: %d\n", id, i);
        sleep(1);
    }

    return NULL;
}

int main() {
    pthread_t thread;
    int id = 112233;

    int err = pthread_create(&thread, NULL, worker, &id);
    if (err) {
        fprintf(stderr, "pthread: %s\n", strerror(err));
        exit(1);
    }

    for (int i = 0; i < 5; ++i) {
        printf("Main: %d\n", i);
        sleep(1);
    }

    pthread_join(thread, NULL);
    return 0;
}
