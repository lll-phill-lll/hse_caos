#include <unistd.h>
#include <stdio.h>

int main() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }
    if (pid > 0) {
        printf("Hello from parent. My pid: %d, childs pid: %d\n", getpid(), pid);
    }
    if (pid == 0) {
        printf("Hello from child. My pid: %d, parents pid: %d\n", getpid(), getppid());
    }
    sleep(1000);
    // какое сообщение напечатается первым?
}
