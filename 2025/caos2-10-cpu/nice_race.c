#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <sched.h>

static volatile sig_atomic_t run = 1;

void handler(int) {
    run = 0;
}

void run_worker(int nice_v, int fd, int sec) {
    if (nice(nice_v) == -1) {
        perror("nice");
        _exit(1);
    }

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    sched_setaffinity(0, sizeof(mask), &mask);

    signal(SIGALRM, handler);
    alarm(sec);

    long long counter = 0;
    while (run) {
        counter++;
    }

    write(fd, &counter, sizeof(counter));
    close(fd);
    _exit(0);
}

int main() {
    int seconds = 3;

    int pa[2], pb[2];
    if (pipe(pa) < 0 || pipe(pb) < 0) {
        perror("pipe");
        return 1;
    }

    pid_t pid_a = fork();
    if (pid_a == 0) {
        close(pa[0]);
        close(pb[0]);
        close(pb[1]);
        run_worker(0, pa[1], seconds);
    }

    pid_t pid_b = fork();
    if (pid_b == 0) {
        close(pa[0]);
        close(pa[1]);
        close(pb[0]);
        run_worker(19, pb[1], seconds);
    }

    close(pa[1]);
    close(pb[1]);

    long long ca = 0, cb = 0;
    int status;

    waitpid(pid_a, &status, 0);
    waitpid(pid_b, &status, 0);

    read(pa[0], &ca, sizeof(ca));
    read(pb[0], &cb, sizeof(cb));
    close(pa[0]);
    close(pb[0]);

    printf("[nice  0]: %lld iters\n", ca);
    printf("[nice 19]: %lld iters\n", cb);

    printf("A/B: %.2fx\n", (double)ca / cb);

    return 0;
}
