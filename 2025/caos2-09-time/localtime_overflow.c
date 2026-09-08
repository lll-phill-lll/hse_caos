#include <time.h>
#include <stdio.h>
#include <limits.h>

int main() {
    time_t t = LONG_MAX / 1024;
    t += 1;
    printf("%ld\n", t);

    char* ctime_res = ctime(&t);
    printf("%s\n", ctime_res);
}
