#include <sys/mman.h>
#include <iostream>
#include <cstring>

int main() {
    void* mp = mmap(0, sizeof(int), PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (mp == MAP_FAILED) {
        std::cerr << __LINE__ << std::strerror(errno) << std::endl;
        exit(1);
    }
    int* num = static_cast<int*>(mp);

    for (int i = 0; i < 10; ++i) {
        std::cout << ++*num << std::endl;
    }

    if (munmap(mp, sizeof(int)) == -1) {
        std::cerr << __LINE__ << std::strerror(errno) << std::endl;
        exit(1);
    }

    return 0;
}
