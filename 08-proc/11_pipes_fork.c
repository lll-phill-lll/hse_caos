#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>


int main() {
    int fds[2];  // массив для файловых дескрипторов канала
    pipe(fds);
    pid_t pid1 = fork();
    if (!pid1) {
        // первый процесс: ls
        // перенаправить записывающую сторону канала
        dup2(fds[1], STDOUT_FILENO);
        // закрыть файловые дескрипторы канала, они больше не нужны
        close(fds[0]); close(fds[1]);
        execlp("ls", "ls", "-l", NULL);
        // обязательно после exec должен быть _exit
        _exit(1);
    }
    // файловый дескриптор для записи в канал больше не нужен
    close(fds[1]);
    pid_t pid2 = fork();
    if (!pid2) {
        // второй процесс: wc
        // перенаправить считывающую сторону канала
        dup2(fds[0], STDIN_FILENO);
        // файловый дескриптор канала больше не нужен
        close(fds[0]);
        execlp("wc", "wc", "-l", NULL);
        _exit(1);
    }
    // отцу не нужен файловый дескриптор для чтения из канала
    close(fds[0]);
    // отец должен дождаться завершения обоих сыновей
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}
