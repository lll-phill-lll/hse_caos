#include <locale.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // или "fr_FR.UTF-8"
    setlocale(LC_TIME, "ru_RU.UTF-8");

    time_t t = time(NULL);
    struct tm *tmp = localtime(&t);
    
    char outstr[200]; 
    if (!strftime(outstr, sizeof(outstr), argv[1], tmp)) {
       fprintf(stderr, "strftime returned 0");
       exit(EXIT_FAILURE);
    }

    printf("%s\n", outstr);
}
