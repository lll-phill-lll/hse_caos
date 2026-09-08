#define _XOPEN_SOURCE
#include <stdio.h>
#include <string.h>
#include <time.h>


int main(int argc, char *argv[]) {
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));

    strptime(argv[1], "%Y-%m-%d %H:%M:%S", &tm);

    tm.tm_hour += 100;
    mktime(&tm);

    char date[255];
    strftime(date, sizeof(date), "%d %b %Y %H:%M, %a", &tm);
    printf("%s\n", date);
}
