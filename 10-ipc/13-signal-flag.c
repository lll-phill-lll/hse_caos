#include <signal.h>
#include <stdio.h>
#include <unistd.h>

volatile sig_atomic_t flag = 0;

void sigint_handler(int n) {
    if (flag == 1) {
        _exit(0);
    }
    flag = 1;
}

int main() {
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sa.sa_flags = SA_NODEFER;
    sigaction(SIGINT, &sa, NULL);

    while (1) {
        if (flag) {
            printf("Ctrl-C pressed\n");
        } else {
            printf("No signal\n");
        }
        sleep(1);
    }
    return 0;
}
