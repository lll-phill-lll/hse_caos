#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void handler(int sig, siginfo_t *siginfo, void *context) {
    printf("Sending PID: %d, UID: %d\n", siginfo->si_pid, siginfo->si_uid);
}

int main() {
    fprintf(stderr, "pid: %i parent pid: %i\n", getpid(), getppid());

    struct sigaction sa;
    sa.sa_sigaction = handler;

    sa.sa_flags = SA_SIGINFO;

    sigaction(SIGINT, &sa, NULL);

    pause();

    return 0;
}
