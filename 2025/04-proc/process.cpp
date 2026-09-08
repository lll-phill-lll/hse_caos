#include <iostream>
#include <unistd.h>

int main() {
    std::cout << "Process Id (pid): " << getpid() << std::endl;
    std::cout << "Parent Pid: " << getppid() << std::endl;

    sleep(1000);
    return 0;
}

