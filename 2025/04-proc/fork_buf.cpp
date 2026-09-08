#include <iostream>
#include <unistd.h>

int main() {
    std::cout << "hello world!";

    auto pid = fork();
    if (pid == 0) {
        _exit(0);
    }

    return 0;
}

