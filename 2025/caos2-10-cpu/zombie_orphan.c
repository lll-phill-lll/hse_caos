#include <unistd.h>

int main () {
    int pid = fork();
    if (pid == 0) {
        sleep(100000);
        _exit(0);
    }
    sleep(1000000);
}
