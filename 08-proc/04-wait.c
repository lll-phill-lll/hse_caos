#include <unistd.h>
#include <stdio.h>

#include <sys/wait.h>

void child_function() {
    sleep(5);
    printf("[child][%d] running\n", getpid());
    // what if we forget this exit?
    // watch -n1 pstree -Tp <zsh pid>
    _exit(0);
}

int main() {
    int child_num = 10;
    sleep(1);

    for (int i = 0; i < child_num; ++i) {
        if (fork() == 0) {
            child_function();
        }
    }

    for (int i = 0; i < child_num; ++i) {
        wait(NULL);
    }

    printf("[parent] All processes finished\n");
    return 0;
}

