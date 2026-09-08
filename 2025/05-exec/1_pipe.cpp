#include <iostream>
#include <array>
#include <csignal>
#include <cstdlib>
#include <unistd.h>
#include <cstdio>

int main() {
    int fds[2];  // массив из файловых дискрипторов
                 // fds[0] - отвечает за чтение, fds[1] - отвечает за запись
    if (pipe(fds) == -1) {  // превращаем файловые дискрипторы в канал и ловим ошибку
        std::perror("pipe");
        std::exit(1);
    }
    // 0 1 2 [3, 4]
    /*-------------------п-р-и-м-е-р--1-------------------*/

    // write/read
    // {
    //     char wbuf[] = "hello";
    //     char rbuf[6];

    //     write(fds[1], wbuf, sizeof(wbuf));  // запись, поэтому fds[1]

    //     ssize_t read_size =
    //         read(fds[0], rbuf, sizeof(rbuf));  // чтение, поэтому fds[0]
    //     std::cout << "read bytes: " << read_size << std::endl;  // смотрим сколько прочитали
    //     std::cout << "read from pipe: " << rbuf << std::endl;
    // }

    /*-------------------п-р-и-м-е-р--2-------------------*/
    // {
    //     char rbuf[6] = {};
    //     close(fds[1]);

    //     // вот тут он ждет пока кто-то что-то не напишет
    //     ssize_t read_size = read(fds[0], rbuf, sizeof(rbuf));
    //     std::cout << "read bytes: " << read_size << std::endl;
    //     std::cout << "read from pipe: " << rbuf << std::endl;
    // }

    /*-------------------п-р-и-м-е-р--3-------------------*/
    // {
    //     // yes caos | head
    //     // signal(SIGPIPE, SIG_IGN);
    //     char wbuf[] = "hello";
    //     close(fds[0]);

    //     ssize_t res = write(fds[1], wbuf, sizeof(wbuf));
    //     if (res < 0) {
    //         std::perror("write");
    //     } else {
    //         std::cout << "success\n";
    //     }
    // }

    /*-------------------п-р-и-м-е-р--4-------------------*/
    //
    //    // если считанные данные больше, чем буфер, куда мы записываем,
    //    // то выведется кол-во элементов, равное размеру буфера выода
    //
    {
        char wbuf[] = "hello world!";

        char rbuf[2];
        write(fds[1], wbuf, sizeof(wbuf));
        close(fds[1]);

        ssize_t read_size;
        while((read_size = read(fds[0], rbuf, sizeof(rbuf))) > 0) {
            std::cout << "read from pipe: ";
            std::cout.write(rbuf, read_size) << std::endl;
        }
    }
    close(fds[0]);
    close(fds[1]);
}

