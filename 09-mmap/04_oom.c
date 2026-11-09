#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    size_t size = 10ULL * 1024 * 1024 * 1024;

    char *ptr = mmap(NULL, size,
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS,
                     -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    printf("PID: %d\n", getpid());
    printf("Mapped 10 GiB, press Enter...\n");
    getchar();

    if (argc > 1) {
        long page = sysconf(_SC_PAGESIZE);
        for (size_t i = 0; i < size; i += page) {
            ptr[i] = '1';
        }

        printf("Memory initialized, press Enter...\n");
        getchar();
    }

    munmap(ptr, size);
    return 0;
}
