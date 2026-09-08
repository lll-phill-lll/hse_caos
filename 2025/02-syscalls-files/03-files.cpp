#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>


int main(int argc, char **argv) {
    const char *src = (argc > 1) ? argv[1] : "input.txt";
    const char *dst = (argc > 2) ? argv[2] : "output.txt";
    getchar();
    int in_fd = open(src, O_RDONLY);
    getchar();
    if (in_fd == -1) {
        std::cerr << errno << " " << std::strerror(errno) << std::endl;
        return 1;
    }

    // O_APPEND - дописывать в конец файла
    // O_CREAT - создать файл, если его не существует
    // O_TRUNK - очистить файл, если он уже существует
    // O_RDONLY - открыть файл для чтения
    // O_WRONLY - открыть файл для записи
    // O_RDWR - открыть файл для чтения и записи
    //
    // если указан O_CREATE, то нужно также указать
    // права, с которыми будет создан файл
    int out_fd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd == -1) {
        std::cerr << std::strerror(errno) << std::endl;
        close(in_fd);
        return 1;
    }

    constexpr size_t BUF_SZ = 16;
    char buf[BUF_SZ];

    ssize_t r;
    while ((r = read(in_fd, buf, BUF_SZ)) != 0) {
        if (r == -1) {
            std::cerr << std::strerror(errno) << std::endl;
            close(in_fd);
            close(out_fd);
            return 1;
        }

        if (!write(STDOUT_FILENO, buf, static_cast<size_t>(r))) {
            close(in_fd);
            close(out_fd);
            return 1;
        }

        if (!write(out_fd, buf, static_cast<size_t>(r))) {
            close(in_fd);
            close(out_fd);
            return 1;
        }
    }

    if (close(in_fd) == -1) {
        std::cerr << std::strerror(errno) << std::endl;
    }
    if (close(out_fd) == -1) {
        std::cerr << std::strerror(errno) << std::endl;
    }

    return 0;
}

