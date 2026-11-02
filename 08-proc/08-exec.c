#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    // // передаем путь до бинаря, название программы, кучу аргументов,
    // заканчивающиеся NULL
    // int status = execl("/usr/bin/sdasdjsakd", "echo", "hello world", NULL);
    // printf("MISHA\n");
    // perror("execlp");

    // // передаем название программы(сам там ищет где лежит), кучу аргументов,
    // заканчивающиеся NULL
    // strace
    int status = execlp("dshl", "echo", "hello world", NULL);
    perror("");

    // // передаем название программы(сам там ищет где лежит),  массив из кучи
    // аргументоа, оканчивающийся NULL
    // char* params[] = {"echo", "hello world", NULL};
    // int status = execvp("echo", params);

    // // добавляется последним элементом массив из перемпенных окружения,
    // массив тоже оканчиваеося NULL
    // char* envs[] = {"PWD=hello", NULL};
    // int status = execle("/usr/bin/printenv", "printenv", NULL, envs);

    // // открывает shell c аргументом -c, запускает команду cmd, которую мы
    // передаем ему
    // // но таким образом мы создаем еще два ребенка, одного сами, когда
    // вызываем shell,
    // // второго вызывает shell, и к нему мы не имеем доступа.

    // char* cmd = "sleep 10; ls -la";
    // int status = execl("/bin/sh", "sh", "-c", cmd, NULL);

    // за окраску отвечают переменные окружения
    // можно и shell и то и то умеет  красить символы в цвет
    // char* cmd = "echo -e \"${RED}hello${NC} world\"";
    // setenv("RED", "\033[0;31m", 1);
    // setenv("NC", "\033[0m", 1);
    // int status = execl("/bin/bash", "sh", "-c", cmd, NULL);
    // no need

    // int status = execl("./hello_world.sh", "HELLO WORLD", NULL);
    // perror("");
}
