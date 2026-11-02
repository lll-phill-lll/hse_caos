#include <unistd.h>

#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        sleep(500);
        _exit(0);
    }

    sleep(10);
    // wait(NULL);

    return 0;
}

