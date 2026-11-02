// github.com/victor-yacovlev/fpmi-caos/tree/master/practice/fork
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    pid_t pid;
    do {
        pid = fork();
        usleep(500000);
    } while (-1 != pid);

    // printf("Process %d reached out limit on processes\n", getpid());
    while (1) {
        sched_yield();
    }
}
