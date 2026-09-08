#include <stdio.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void do_long_work() {
    for (int i = 0; i != 10000000; ++i) {
        for (int j = 0; j < 5; ++j) {
            int pid = getppid();
        }
    }
}

int main() {
    do_long_work();

    for (int i = 0; i < 5; ++i) {
        if (!fork()) {
            do_long_work();
            _exit(0);
        }
    }

    for (int i = 0; i < 5; ++i) {
        wait(NULL);
    }

    struct tms t;
    times(&t);

    int clock_ticks = sysconf(_SC_CLK_TCK);

    printf("Clock ticks:\t%d\n", clock_ticks);
    printf("User time:\t%lu\nSystem time:\t%lu\n", t.tms_utime, t.tms_stime);
    printf("User time(child):\t%lu\nSystem time(child):\t%lu\n", t.tms_cutime, t.tms_cstime);
}

