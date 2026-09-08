#include <unistd.h>
#include <stdio.h>

int main() {
    if (isatty(fileno(stdout)))
        printf("Terminal\n");
    else
        printf("Pipe or file\n");
}

