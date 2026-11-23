#include <pthread.h>
#include <stdio.h>

int amount = 100;

void* spend(void* arg) {
    amount -= 30;
    return NULL;
}

void* add(void* arg) {
    amount += 10;
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, spend, NULL);
    pthread_create(&t2, NULL, add, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Amount: %d\n", amount);
    return 0;
}
