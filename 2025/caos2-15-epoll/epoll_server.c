#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 33333
#define MAX_EVENTS 2 

uint64_t total_requests = 0;
int epoll_fd;

void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void handle_connection(int server_fd) {
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        printf("New connection. fd = %d\n", client_fd);

        if (client_fd < 0) {
            if (errno == EAGAIN) {
                break;
            }

            perror("accept");
            break;
        }

        set_nonblocking(client_fd);

        struct epoll_event ev;

        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = client_fd;

        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
    }
}

void handle_timer(int timer_fd) {
    uint64_t expirations;

    ssize_t n = read(timer_fd, &expirations, sizeof(expirations));

    printf("[STATS] Total requests: %lu\n", total_requests);
}

void handle_client(int client_fd) {
    printf("Handle client\n");
    char buf[1024];

    ssize_t n = read(client_fd, buf, sizeof(buf));

    if (n > 0) {
        ++total_requests;

        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: 2\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "OK";

        write(client_fd, response, strlen(response));
    }

    close(client_fd);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));

    listen(server_fd, 10);

    set_nonblocking(server_fd);

    int timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);

    struct itimerspec timer_spec;
    memset(&timer_spec, 0, sizeof(timer_spec));

    timer_spec.it_value.tv_sec = 5;
    timer_spec.it_interval.tv_sec = 5;

    timerfd_settime(timer_fd, 0, &timer_spec, NULL);

    epoll_fd = epoll_create1(0);

    struct epoll_event ev;

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = server_fd;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = timer_fd;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, timer_fd, &ev);

    printf("HTTP server (epoll) on port %d\n", PORT);

    struct epoll_event events[MAX_EVENTS];

    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;

            if (fd == server_fd) {
                handle_connection(server_fd);
            } else if (fd == timer_fd) {
                handle_timer(timer_fd);
            } else {
                handle_client(fd);

                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
            }
        }
    }

    close(server_fd);
    close(epoll_fd);
    close(timer_fd);

    fprintf(stderr, "Total: %lu\n", total_requests);

    return 0;
}
