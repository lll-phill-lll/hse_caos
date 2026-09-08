#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#define BUFFER_SIZE 2
#define MAX_EVENTS 2


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file1> \n", argv[0]);
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY | O_NONBLOCK);

    char buf[BUFFER_SIZE];

    int epoll_fd = epoll_create1(0);

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;

    event.data.fd = fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);

    while (1) {
        struct epoll_event events[MAX_EVENTS];
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < num_events; i++) {
            int efd = events[i].data.fd;
            printf("Event happened\n");

            if (events[i].events & EPOLLIN) {
                ssize_t size;
                while ((size = read(efd, buf, BUFFER_SIZE)) > 0) {
                    printf("[CH]: %.*s\n", (int)size, buf);
                }
            } 

            if (events[i].events & EPOLLHUP) {
                printf("Fd closed");
                // epoll_ctl(epoll_fd, EPOLL_CTL_DEL, efd,NULL);
                close(efd);
                return 0;
            }
        }
        sleep(1);
    }

    close(epoll_fd);

    return 0;
}

