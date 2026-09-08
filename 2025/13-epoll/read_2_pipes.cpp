#include <iostream>
#include <fcntl.h>
#include <unistd.h>

void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <pipe1> <pipe2>" << std::endl;
        return 1;
    }
    
    int fd1 = open(argv[1], O_RDONLY | O_NONBLOCK);
    int fd2 = open(argv[2], O_RDONLY | O_NONBLOCK);


    char buf[1024];
    int open_pipes = 2;
    
    while (open_pipes > 0) {
        ssize_t n1 = read(fd1, buf, sizeof(buf));
        if (n1 > 0) {
            std::cout << "[PIPE1]: ";
            std::cout.write(buf, n1);
        } else if (n1 == 0) {
            close(fd1);
            --open_pipes;
        }
        
        ssize_t n2 = read(fd2, buf, sizeof(buf));
        if (n2 > 0) {
            std::cout << "[PIPE2]: ";
            std::cout.write(buf, n2);
        } else if (n2 == 0) {
            close(fd2);
            --open_pipes;
        }
    }
    
    return 0;
}

