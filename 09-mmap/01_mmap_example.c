#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct stat st;
    fstat(fd, &st);

    size_t size = st.st_size;
    if (size == 0) {
        close(fd);
        return 0;
    }

    char *data = mmap(NULL, size,
                      PROT_READ | PROT_WRITE,
                      MAP_PRIVATE, fd, 0);

    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    close(fd);

    for (size_t i = 0; i < size; ++i) {
        if (data[i] >= '0' && data[i] <= '9') {
            data[i] = '*';
        }
    }

    munmap(data, size);
    return 0;
}
