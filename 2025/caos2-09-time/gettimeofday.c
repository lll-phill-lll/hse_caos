
#include <stdio.h>
#include <sys/time.h>
#include <sys/time.h>


int main() {
    // struct timeval хранит секунды и микросекунды
    struct timeval current_time; 
    // gettimeofday почти как time, но дает более высокую точность
    gettimeofday(&current_time, NULL); 
    // accuracy: 20ms
    printf("sec: %ld, usec: %d\n", current_time.tv_sec, current_time.tv_usec);
}
