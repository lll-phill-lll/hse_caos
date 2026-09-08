#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <thread>
#include <unistd.h>
#include <cerrno>
#include <cstdint>

#define PORT 33333
#define MAX_EVENTS 64

uint64_t total_requests = 0;
int epoll_fd;

void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void handle_connection(int server_fd) {
    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        
        if (client_fd < 0) break;
        
        set_nonblocking(client_fd);
        
        epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = client_fd;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
    }
}

void handle_timer(int timer_fd) {
    uint64_t expirations;
    read(timer_fd, &expirations, sizeof(expirations));
    std::cout << "[STATS] Total requests: " << total_requests << std::endl;
}

void handle_client(int client_fd) {
    char buf[1024];

    ssize_t n = read(client_fd, buf, sizeof(buf));
    if (n > 0) {
        ++total_requests;

        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: 2\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "OK";

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        write(client_fd, response.data(), response.size());

    }
    close(client_fd);
}


int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 128);
    set_nonblocking(server_fd);
    
    int timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    itimerspec timer_spec{};
    timer_spec.it_value.tv_sec = 5;
    timer_spec.it_interval.tv_sec = 5;
    timerfd_settime(timer_fd, 0, &timer_spec, nullptr);
    
    epoll_fd = epoll_create1(0);
    
    epoll_event ev;
    // 
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);
    
    ev.data.fd = timer_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, timer_fd, &ev);
    
    std::cout << "Echo server (epoll) on port " << PORT << std::endl;
    
    epoll_event events[MAX_EVENTS];
    
    while (true) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        
        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;
            
            if (fd == server_fd) {
                handle_connection(server_fd);
            } else if (fd == timer_fd) {
                handle_timer(timer_fd);
            } else {
                handle_client(fd);
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
            }
        }
    }
    
    close(server_fd);
    close(epoll_fd);
    close(timer_fd);

    std::cerr << "Total:" << total_requests << std::endl;
    return 0;
}
