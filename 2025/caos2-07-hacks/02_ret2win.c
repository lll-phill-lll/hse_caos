#include <stdio.h>

void secret() {
    printf("You found the secret function!\n");
}

void vuln() {
    // ret

    char buf[32];
    printf("Input: ");
    scanf("%s", buf);
}

int main() {
    printf("%p, %p\n", (void*)secret, (void*)vuln);
    vuln();
    printf("Returned normally from vuln()\n");
    return 0;
}
