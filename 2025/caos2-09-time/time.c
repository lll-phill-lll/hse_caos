#include <time.h>
#include <stdio.h>

int main() {
    time_t t = time(NULL);
    printf("%ld\n", t / 3600 / 24 % 365);
}
