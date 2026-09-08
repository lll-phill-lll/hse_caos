//   gcc -g -fno-stack-protector -z execstack -no-pie stage1_vuln.c
//
//   gcc -g -fno-stack-protector -no-pie stage1_vuln.c
//

#include <stdio.h>
#include <unistd.h>


void service() {
    // ->
    char name[64];
    char msg[64];
    // code (50)
    //
    //AAAAAAAAAAA
    //
    //addr (ret) -> kk
    //
    // printf("%p\n", &name);
    // printf("%p\n", &msg);

    printf("What is your name? ");
    scanf("%s", name);

    printf("Hello, ");
    printf(name);
    printf("!\n");

    printf("Leave a message: ");
    scanf("%s", msg);

    printf("Thanks! Message received.\n");
}

int main() {
    // ret
    service();
    return 0;
}
