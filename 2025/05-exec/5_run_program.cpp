#include <cstdio>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char** argv) {

    // ---- some code ---- //
    pid_t pid = fork();

    if (pid == 0) {

        int fd = open("output.txt", O_RDWR | O_CREAT, 0777);
        dup2(fd, 1);
        close(fd);
        // 0 stdin
        // 1 output.txt
        // 2 stderr

        // ./a.out /usr/bin/echo
        execl(argv[1], argv[1], nullptr);
        std::perror("exec");
        _exit(1);
    }

    wait(nullptr);

    // ---- some code ---- //
    return 0;
}

