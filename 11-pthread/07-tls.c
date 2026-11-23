#include <pthread.h>
#include <stdio.h>

_Thread_local int counter = 0;

void foo() {
    static _Thread_local int local = 0;

    local++;
    printf("Foo: %d\n", local);
}

void* worker(void* arg) {
    counter++;
    printf("Worker: %d\n", counter);
    for (int i = 0; i < 3; ++i) {
        foo();
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, worker, NULL);
    pthread_create(&t2, NULL, worker, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
