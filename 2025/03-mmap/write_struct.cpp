#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

struct Data {
    int id;
    std::string s;
    // 2 byte
    double value;
};

constexpr int ARRAY_SIZE = 5;

int main() {
    const char* filename = "data.bin";

    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        std::cerr << __LINE__ << std::strerror(errno) << std::endl;
        exit(1);
    }

    size_t dataSize = sizeof(Data) * ARRAY_SIZE;
    if (ftruncate(fd, dataSize) == -1) {
        std::cerr << __LINE__ << std::strerror(errno) << std::endl;
        exit(1);
    }

    void* mp =  mmap(nullptr, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mp == MAP_FAILED) {
        std::cerr << __LINE__ << std::strerror(errno) << std::endl;
        exit(1);
    }
    close(fd);
    
    // ARRAY_SIZE 
    Data* memory = static_cast<Data*>(mp);
    // Data Data Data Data

    Data toWrite = {43, "Hello world", 3.14159};
    std::memcpy(memory, &toWrite, sizeof(Data));
    memory[3].id = 42;

    std::string s;
    char newString[] = "lol kek";
    std::strncpy(memory[4].name, newString, sizeof(newString));

    if (msync(memory, dataSize, MS_SYNC) == -1) {
        std::cerr << __LINE__ << std::strerror(errno) << std::endl;
        exit(1);
    }

    if (munmap(memory, dataSize) == -1) {
        std::cerr << __LINE__ << std::strerror(errno) << std::endl;
        exit(1);
    }

    /// ------------------------

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        std::cerr << __LINE__ << std::strerror(errno) << std::endl;
        exit(1);
    }
    mp = mmap(nullptr, dataSize, PROT_READ, MAP_SHARED, fd, 0);

    if (mp == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    memory = static_cast<Data*>(mp);
    close(fd);

    for (int i = 0; i < ARRAY_SIZE; ++i) {
        std::cout << "id: "    << memory[i].id   << std::endl;
        std::cout << "name: "  << memory[i].name << std::endl;
        std::cout << "value: " << memory[i].value << std::endl;
        std::cout << "--------------" << std::endl;
    }

    if (munmap(memory, dataSize) == -1) {
        perror("munmap");
        return 1;
    }

    return 0;
}

