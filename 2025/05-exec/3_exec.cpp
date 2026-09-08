
#include <iostream>
#include <cstdlib>
#include <unistd.h>

int main(int argc, char* argv[]) {
    // передаем путь до бинаря, название программы, кучу аргументов,
    // заканчивающиеся NULL
    // int status = execl("/usr/bin/echo", "echo", "hello", "world", nullptr);

    // передаем название программы (сам там ищет где лежит), кучу аргументов,
    // заканчивающиеся NULL
    // int status = execlp("lallaalal", "echo", "hello", "world", nullptr);
    // std::perror("");

    // передаем название программы (сам там ищет где лежит), массив из кучи
    // аргументов, оканчивающийся NULL
    // char* params[] = {
    //     (char*)"echo",
    //     (char*)"hello",
    //     (char*)"world",
    //     nullptr
    // };
    // int status = execvp("echo", params);

    // добавляется последним элементом массив из переменных окружения,
    // массив тоже оканчивается NULL
    // char* envs[] = { (char*)"PWD=hello", nullptr };
    // int status = execle("/usr/bin/printenv", "printenv", nullptr, envs);
    // int status = execl("/usr/bin/printenv", "printenv", nullptr);

    // // открывает shell с аргументом -c, запускает команду cmd, которую мы
    // // передаем ему. таким образом мы создаем два ребенка:
    // // одного — сами, когда вызываем shell,
    // // второго — сам shell, и к нему мы не имеем доступа.
    //
    // const char* cmd = "sleep 10; ls -la";
    // int status = execl("/bin/sh", "sh", "-c", cmd, nullptr);

    int status = execl("./hello_world.sh", "HELLO WORLD", nullptr);
    std::perror("");

    return status;
}

