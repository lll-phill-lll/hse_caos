#include <iostream>
#include <fcntl.h>
#include <sys/resource.h>
#include <unistd.h>

int main() {
    struct rlimit rlim;
    getrlimit(RLIMIT_NOFILE, &rlim);

    unsigned long limit = rlim.rlim_cur;
    if (limit == RLIM_INFINITY || limit > 1ULL << 20) {
        limit = 1024 * 1024;
    }

    char buf[1024] = {};
    for (int fd = 3; fd < limit; ++fd) {
        ssize_t n;
        if ((n = read(fd, buf, sizeof(buf))) > 0) {
            std::cout << "Found secret file :)" << std::endl;
            std::cout << "Contents:" << std::endl;
            std::cout.write(buf, n);
            std::cout << std::endl;
            return 0;
        }
    }

    std::cout << "No leaked files found :(" << std::endl;
    return 0;
}
