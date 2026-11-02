#include <unistd.h>
#include <stdio.h>

#include <sys/wait.h>

void child_function() {
    printf("[child][%d] hello\n", getpid());
    sleep(5);
    printf("[child][%d] exit\n", getpid());
    _exit(5);
}

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        child_function();
    }

    int status = 0;
    sleep(5000);
    // waitpid(pid, &status, 0);

    // // Проверка того, завершился ли ребенок нормально
    // if (WIFEXITED(status)) {
    //     printf("Child exit status: %d\n", WEXITSTATUS(status));
    // }
    // // Проверка: был ли ребенок завершен сигналом
    // if (WIFSIGNALED(status)) {
    //     printf("Child exit signal: %d\n", WTERMSIG(status));
    // }

    return 0;
}

