#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
    while (1) {
        int cpu = sched_getcpu();
        if (cpu == -1) {
            perror("sched_getcpu");
            return 1;
        }

        printf("pid=%d cpu=%d\n", getpid(), cpu);
        fflush(stdout);
    }

    return 0;
}
