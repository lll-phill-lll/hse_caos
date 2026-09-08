#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

void child_read(int fds[]) {
    int val;
    while (read(fds[0], &val, sizeof(val)) > 0) {
        std::cout << "[child] read: " << val << std::endl;
    }
    close(fds[0]);

    std::cout << "[child] finished reading\n";
    _exit(0);
}

void parent_write(int fds[]) {
    for (int i = 0; i < 5; ++i) {
        sleep(1);
        int square = i * i;
        std::cout << "[parent] write: " << square << "\n";
        write(fds[1], &square, sizeof(square));
    }
    std::cout << "[parent] finished writing\n";

    close(fds[1]);
}

int main() {
    int fds[2];
    pipe(fds);

    pid_t pid = fork();

    if (pid == 0) {
        close(fds[1]);
        child_read(fds);
    }

    close(fds[0]);
    parent_write(fds);
    waitpid(pid, nullptr, 0);
    return 0;
}
