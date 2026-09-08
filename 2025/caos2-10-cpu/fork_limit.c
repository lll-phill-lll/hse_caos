#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

int main() {
    int counter = 1;
    pid_t pid = 0;

    while (pid == 0) {
        sleep(2);
        pid = fork();
        if (pid != -1) {
            counter++;
        } else {
            perror("error");
        }
    }

    if (pid > 0) {
        waitpid(pid, NULL, 0);
    } else if (pid == -1) {
        printf("%d\n", counter);
        _exit(0);
    }
}
