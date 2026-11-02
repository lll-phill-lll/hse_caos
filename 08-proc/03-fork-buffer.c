#include <unistd.h>
#include <stdio.h>

int main() {
    printf("[%d] I'm printed once", getpid());
    fork();
    printf("[%d] I'm printed twice\n", getpid());
    // Сколько раз напечатается первое сообщение, если убрать \n?
}
