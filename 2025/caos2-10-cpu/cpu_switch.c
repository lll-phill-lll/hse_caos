#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    int last_cpu = -1;

    while (1) {
        int cpu = sched_getcpu();
        if (cpu == -1) {
            perror("sched_getcpu");
            return 1;
        }

        if (cpu != last_cpu) {
            printf("thread moved: %d -> %d\n", last_cpu, cpu);
            fflush(stdout);
            last_cpu = cpu;
        }
    }

    return 0;
}
