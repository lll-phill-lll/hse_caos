#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <pipe1> <pipe2>" << std::endl;
        return 1;
    }
    
    int fd1 = open(argv[1], O_RDONLY);
    int fd2 = open(argv[2], O_RDONLY);
    
    int epoll_fd = epoll_create1(0);
    
    epoll_event ev;
    // Ждём данные для чтения
    ev.events = EPOLLIN | EPOLLHUP;
    
    ev.data.fd = fd1;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd1, &ev);
    
    ev.data.fd = fd2;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd2, &ev);
    // epollfd -> fd1, fd2
    
    char buf[1024];
    int open_pipes = 2;
    
    while (open_pipes > 0) {
        epoll_event events[2];
        
        int nfds = epoll_wait(epoll_fd, events, 2, -1);
        
        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;
            std::string name = (fd == fd1) ? "PIPE1" : "PIPE2";

            if (events[i].events & EPOLLHUP) {
                std::cout << name << " closed" << std::endl;
            }

            ssize_t n = read(fd, buf, sizeof(buf));
            if (n > 0) {
                std::cout << "[" << name << "]: ";
                std::cout.write(buf, n);
            } else if (n == 0) {
                // EOF — pipe закрыт
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
                close(fd);
                --open_pipes;
            }
        }
    }
    
    close(epoll_fd);
    return 0;
}

