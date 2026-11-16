#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

struct someSharedData {
    int value;
    pid_t writer_pid;
};

int main() {
    struct someSharedData* shared = mmap(NULL, sizeof(*shared),
            PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    pid_t child = fork();
    if (child == 0) {
        shared->value = 42;
        shared->writer_pid = getpid();
        _exit(0);
    }

    waitpid(child, NULL, 0);

    printf("value = %d, writer_pid = %ld\n", shared->value,
            (long)shared->writer_pid);

    munmap(shared, sizeof(*shared));
    return 0;
}

