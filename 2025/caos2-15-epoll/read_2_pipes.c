#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

void make_read_non_block(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file1> <file2>\n", argv[0]);
        exit(1);
    }

    int fd1 = open(argv[1], O_RDONLY | O_NONBLOCK);
    int fd2 = open(argv[2], O_RDONLY | O_NONBLOCK);

    char buf[BUFFER_SIZE];

    while (1) {
        int size1 = read(fd1, buf, BUFFER_SIZE);
        if (size1 > 0) {
            printf("[CH1]: %.*s", size1, buf);
        }

        int size2 = read(fd2, buf, BUFFER_SIZE);
        if (size2 > 0) {
            printf("[CH2]: %.*s", size2, buf);
        }

        if (!size1 && !size2) break;

    }

    close(fd1);
    close(fd2);

    return 0;
}

