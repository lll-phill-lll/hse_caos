#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void handler(int n) {
    char msg[] = "got signal\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s DELAY STRING\n", argv[0]);
        return 1;
    }
    struct sigaction sa = {};
    // sa.sa_handler = handler;
    // sa.sa_handler = SIG_IGN;
    sa.sa_handler = SIG_DFL;
    sa.sa_flags = SA_RESTART;

    sigaction(SIGINT, &sa, NULL);

    int delay = atoi(argv[1]);
    int counter = 1;

    while (1) {
        sleep(delay);  // при вызове SIGINT таймер sleep обнуляется
        printf("[%d] %d: %s\n", getpid(), counter++, argv[2]);
    }
}
