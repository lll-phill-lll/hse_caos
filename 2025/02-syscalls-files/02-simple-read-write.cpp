#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>

int main() {
    constexpr size_t BUF_SZ = 4;
    char buf[BUF_SZ];


    // 100
    // int total_Read = 0;
    // char buf[100];

    // while (total_read != 100) {
    //     total_read += read(9, buf[total_read], 100 - total_read);
    //     // 0
    //     // -1
    // }

    while (true) {
        ssize_t n = read(STDIN_FILENO, buf, BUF_SZ);
        if (n == 0) {
            break;
        }
        if (n == -1) {
            std::cerr << "read() failed: " << std::strerror(errno) << std::endl;
            return 1;
        }

// NOTE! write не обязан за один раз записать весь буффер.
// Для правильной работы тут должен быть цикл, который продолжает
// делать write, пока суммарное количество записанных байт не равна
// количеству байт, которое мы хотели записать
        if (write(STDOUT_FILENO, buf, n) <= 0) {
            std::cerr << "write() failed: " << std::strerror(errno) << std::endl;
            return 1;
        }
    }

    return 0;
}

