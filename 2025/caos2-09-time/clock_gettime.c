#include <time.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    struct timespec ts;
    // с помощью clock_gettime мы получаем более точное время
    //
    while(1) {
        // clock_gettime(CLOCK_REALTIME, &ts);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);

        printf("\rsec: %ld, nsec: %ld", ts.tv_sec, ts.tv_nsec);
        fflush(stdout);
        sleep(1);

    }
}
