#include <stdio.h>
#include <sys/mman.h>

int main() {
    int *num = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (num == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    for (int i = 0; i < 10; ++i) {
        printf("%d\n", ++*num);
    }

    munmap(num, sizeof(int));
    return 0;
}

