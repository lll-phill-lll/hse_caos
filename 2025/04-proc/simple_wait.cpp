#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

void child_function() {
    sleep(10);
    std::cout << "[child][" << getpid() << "] running" << std::endl;
    // what if we forget this exit?
    // watch -n1 pstree -Tp
    _exit(0);
}

int main() {
    int child_num = 5;
    sleep(10);

    for (int i = 0; i < child_num; ++i) {
        if (fork() == 0) {
            child_function();
        }
    }

    for (int i = 0; i < child_num+1; ++i) {
        wait(nullptr);
    }

    std::cout << "[parent] all processes finished" << std::endl;
    return 0;
}

