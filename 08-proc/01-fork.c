#include <unistd.h>
#include <stdio.h>

int main() {
    fork();
    printf("Hello from %d\n", getpid());
    sleep(1000);
}
