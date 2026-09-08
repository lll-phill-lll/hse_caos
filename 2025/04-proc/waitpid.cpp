#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

void child_function() {
    std::cout << "child pid = " << getpid() << std::endl;
    sleep(500);
    std::cout << "hello world" << std::endl;
    _exit(5);
}

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        child_function();
    }

    int status = 0;
    waitpid(pid, &status, 0);

    // Проверка того, завершился ли ребенок нормально
    if (WIFEXITED(status)) {
        std::cout << "Child exit status: " << WEXITSTATUS(status) << std::endl;
    }
    // Проверка: был ли ребенок завершен сигналом
    if (WIFSIGNALED(status)) {
        std::cout << "Child exit signal: " << WTERMSIG(status) << std::endl;
    }

    return 0;
}

