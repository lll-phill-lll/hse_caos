
#define _GNU_SOURCE

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
#define MAX_EVENTS 64

static uint64_t total_requests = 0;
static int epoll_fd;

static void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        exit(1);
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        exit(1);
    }
}

static void handle_connection(int server_fd) {
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        memset(&client_addr, 0, sizeof(client_addr));

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }

            perror("accept");
            break;
        }

        set_nonblocking(client_fd);

        struct epoll_event ev;
        memset(&ev, 0, sizeof(ev));

        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = client_fd;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
            perror("epoll_ctl client_fd");
            close(client_fd);
        }
    }
}

static void handle_timer(int timer_fd) {
    uint64_t expirations;

    ssize_t n = read(timer_fd, &expirations, sizeof(expirations));
    if (n < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("read timer_fd");
        }
        return;
    }

    printf("[STATS] Total requests: %lu\n", total_requests);
}

static void sleep_ms(long ms) {
    struct timespec ts;

    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;

    nanosleep(&ts, NULL);
}

static void handle_client(int client_fd) {
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

        sleep_ms(10);

        ssize_t written = write(client_fd, response, strlen(response));
        if (written < 0) {
            perror("write");
        }
    }

    close(client_fd);
}

int main(void) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(server_fd);
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 128) == -1) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    set_nonblocking(server_fd);

    int timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (timer_fd == -1) {
        perror("timerfd_create");
        close(server_fd);
        return 1;
    }

    struct itimerspec timer_spec;
    memset(&timer_spec, 0, sizeof(timer_spec));

    timer_spec.it_value.tv_sec = 5;
    timer_spec.it_interval.tv_sec = 5;

    if (timerfd_settime(timer_fd, 0, &timer_spec, NULL) == -1) {
        perror("timerfd_settime");
        close(timer_fd);
        close(server_fd);
        return 1;
    }

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        close(timer_fd);
        close(server_fd);
        return 1;
    }

    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));

    ev.events = EPOLLIN;
    ev.data.fd = server_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        perror("epoll_ctl server_fd");
        close(epoll_fd);
        close(timer_fd);
        close(server_fd);
        return 1;
    }

    memset(&ev, 0, sizeof(ev));

    ev.events = EPOLLIN;
    ev.data.fd = timer_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, timer_fd, &ev) == -1) {
        perror("epoll_ctl timer_fd");
        close(epoll_fd);
        close(timer_fd);
        close(server_fd);
        return 1;
    }

    printf("HTTP server (epoll) on port %d\n", PORT);

    struct epoll_event events[MAX_EVENTS];

    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        if (nfds == -1) {
            if (errno == EINTR) {
                continue;
            }

            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;

            if (fd == server_fd) {
                handle_connection(server_fd);
            } else if (fd == timer_fd) {
                handle_timer(timer_fd);
            } else {
                handle_client(fd);

                if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
                    /*
                     * Тут может быть EBADF, потому что fd уже закрыт.
                     * Для этого конкретного кода это не страшно.
                     */
                    if (errno != EBADF) {
                        perror("epoll_ctl DEL client_fd");
                    }
                }
            }
        }
    }

    close(server_fd);
    close(epoll_fd);
    close(timer_fd);

    fprintf(stderr, "Total: %lu\n", total_requests);

    return 0;
}
