#include <iostream>
#include <unistd.h>

int main() {
    std::cout << "I'm printed once" << std::endl;

    pid_t pid = fork();

    std::cout << "I'm printed twice" << std::endl;

    if (pid == 0) {
        std::cout << "child" << std::endl;
    } else if (pid > 0) {
        std::cout << "parent" << std::endl;
    }


    while (true) {
        std::cout << "[" << getpid() 
                  << "] pid returned after fork: " << pid
                  << ", parent pid: " << getppid() << std::endl;
        sleep(100);
    }

    return 0;
}

