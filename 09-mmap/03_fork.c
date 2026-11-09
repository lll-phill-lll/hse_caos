#include <stdio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int *value = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (value == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    *value = 42;

    pid_t pid = fork();

    if (pid == 0) {
        printf("[Chld][%d]: addr=%p, value=%d\n",
               getpid(), (void*)value, *value);

        *value = 100;

        printf("[Chld][%d]: addr=%p, value=%d\n",
               getpid(), (void*)value, *value);

        _exit(0);
    }

    waitpid(pid, NULL, 0);

    printf("[Prnt][%d]: addr=%p, value=%d\n",
           getpid(), (void *)value, *value);

    munmap(value, sizeof(int));
    return 0;
}

