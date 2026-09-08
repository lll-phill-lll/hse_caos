#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

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

    int fd1 = open(argv[1], O_RDONLY);
    int fd2 = open(argv[2], O_RDONLY);

    char buf[BUFFER_SIZE];

    fd_set readfds;
    int maxfd = (fd1 > fd2) ? fd1 : fd2;
    maxfd++;


    int fds = 2;
    while (fds) {
        FD_ZERO(&readfds);
        FD_SET(fd1, &readfds);
        FD_SET(fd2, &readfds);

        // 1024
        int ready = select(maxfd, &readfds, NULL, NULL, NULL);
        if (ready == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(fd1, &readfds)) {
            ssize_t size1 = read(fd1, buf, BUFFER_SIZE);
            if (size1 > 0) {
                printf("[CH1]: %.*s", (int)size1, buf);
            } else {
                --fds;
                // closed/error - don't add this fd to the set
            }
        }

        if (FD_ISSET(fd2, &readfds)) {
            ssize_t size2 = read(fd2, buf, BUFFER_SIZE);
            if (size2 > 0) {
                printf("[CH2]: %.*s", (int)size2, buf);
            } else {
                --fds;
                // closed/error - don't add this fd to the set
            }
        }
    }

    close(fd1);
    close(fd2);

    return 0;
}

