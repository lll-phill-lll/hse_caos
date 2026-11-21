#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

// структура с полями
struct somedata {
    int a;
    int b;
    int c;
};

struct somedata sd;

void handler(int n) {
    char str[] = "a: xx, b: xx, c: xx\n";
    str[3] = sd.a / 10 + '0';
    str[4] = sd.a % 10 + '0';
    str[10] = sd.b / 10 + '0';
    str[11] = sd.b % 10 + '0';
    str[17] = sd.c / 10 + '0';
    str[18] = sd.c % 10 + '0';
    write(STDOUT_FILENO, str, sizeof(str) - 1);
    sd.a++;
    sleep(1);
    sd.b++;
    sleep(1);
    sd.c++;
}

int main() {
    struct sigaction sa = {};
    sa.sa_handler = handler;
    sa.sa_flags = SA_NODEFER;
    sigaction(SIGINT, &sa, NULL);

    while(1) {

        sleep(1);
        sd.a++;
        sleep(1);
        sd.b++;
        sleep(1);
        sd.c++;
        printf("[%d] a: %d, b: %d, c: %d\n", getpid(), sd.a, sd.b, sd.c);


    }
}
