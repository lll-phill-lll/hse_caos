#include <iostream>
#include <unistd.h>

int main() {
    int x = 100;
    sleep(10);

    pid_t pid = fork();
    if (pid == 0) {
        sleep(30);
        std::cout << "[Child] x = " << x
                  << " ppid: " << getppid() << std::endl;
        _exit(0);
    }

    std::cout << "[Parent] x = " << x << std::endl;
    x = 1000;
    return 0;
}

