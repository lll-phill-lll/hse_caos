
#include <iostream>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>


constexpr uint64_t DATA_SIZE = 10ULL * 1024 * 1024 * 1024;

int main() {
    const char* filename = "data.bin";

    int fd = open(filename, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        std::cerr << __LINE__ << std::strerror(errno) << std::endl;
        exit(1);
    }

    if (ftruncate(fd, DATA_SIZE) == -1) {
        std::cerr << __LINE__ << std::strerror(errno) << std::endl;
        exit(1);
    }

    void* mp =  mmap(nullptr, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (mp == MAP_FAILED) {
        std::cerr << __LINE__ << std::strerror(errno) << std::endl;
        exit(1);
    }
    close(fd);

    char* data = static_cast<char*>(mp);

    for (uint64_t i = 0; i < DATA_SIZE; ++i) {
        data[i] = '*';
    }
    

    if (munmap(mp, DATA_SIZE) == -1) {
        std::cerr << "munmap: " << std::strerror(errno) << std::endl;
        exit(1);
    }

    return 0;
}
