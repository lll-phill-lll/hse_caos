// Level-triggered (по умолчанию):
//   - epoll_wait возвращает fd ПОКА есть данные
//   - Если не прочитали всё — снова получим событие
//
// Edge-triggered (EPOLLET):
//   - epoll_wait возвращает fd ТОЛЬКО при изменении состояния
//   - Нужно читать ВСЁ до EAGAIN, иначе пропустим данные

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <cerrno>
#include <cstring>

constexpr int BUF_SIZE = 4;

void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <pipe>" << std::endl;
        return 1;
    }
    
    int fd = open(argv[1], O_RDONLY);
    set_nonblocking(fd);
    
    int epoll_fd = epoll_create1(0);
    
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = fd;
    
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    
    char buf[BUF_SIZE];
    
    while (true) {
        epoll_event event;
        int nfds = epoll_wait(epoll_fd, &event, 1, -1);
        
        std::cout << "Event received!" << std::endl;
        
        if (event.events & EPOLLHUP) {
            std::cout << "Pipe closed (EPOLLHUP)" << std::endl;
            break;
        }


        /// 1234567890// sleep(10)  1234567890
        if (event.events & EPOLLIN) {
            ssize_t n;
            while ((n = read(fd, buf, BUF_SIZE)) > 0) {
                std::cout << "Read " << n << " bytes: ";
                std::cout.write(buf, n);
                std::cout << std::endl;
            }
           //  else if (n == 0) {
           //      std::cout << "EOF" << std::endl;
           //      break;
           //  }
        }
        
        sleep(1);
    }
    
    close(fd);
    close(epoll_fd);
    return 0;
}

