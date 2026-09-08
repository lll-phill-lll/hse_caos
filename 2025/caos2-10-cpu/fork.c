#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // sleep(10000);
        _exit(0);
    }

    sleep(1000000);
    return 0;
}
