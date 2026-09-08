#include <iostream>
#include <string>
#include <format>
#include <system_error>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << std::format("Usage: {} NODE SERVICE", argv[0]) << std::endl;
        return 1;
    }

    const std::string node    = argv[1];
    const std::string service = argv[2];

    addrinfo hints{};
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* addrs = nullptr;
    int res = getaddrinfo(node.c_str(), service.c_str(), &hints, &addrs);
    if (res != 0) {
        std::cerr << std::format("getaddrinfo: {}", gai_strerror(res)) << std::endl;
        return 1;
    }

    // Берём первый результат из списка
    int fd = socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);
    if (fd < 0) {
        std::cerr << std::format("socket error: {}\n",
                                 std::system_category().message(errno));
        freeaddrinfo(addrs);
        return 1;
    }

    res = connect(fd, addrs->ai_addr, addrs->ai_addrlen);
    if (res < 0) {
        std::cerr << std::format("connect error: {}\n",
                                 std::system_category().message(errno));
        close(fd);
        freeaddrinfo(addrs);
        return 1;
    }

    // Формируем простой HTTP/1.0 запрос
    // CRLF - carry return + line feed
    std::string request = std::format(
        "GET / HTTP/1.0\r\nHost: www.{}\r\n\r\n", node
    );

    // std::string request = "Hi!\n";

    // send
    // // while
    ssize_t sent = write(fd, request.c_str(), request.size());
    if (sent < 0) {
        std::cerr << std::format("send error: {}\n",
                                 std::system_category().message(errno));
        close(fd);
        freeaddrinfo(addrs);
        return 1;
    }

    // надо обрабатывать ошибки
    char c;
    while (read(fd, &c, 1) == 1) {
        write(STDOUT_FILENO, &c, 1);
    }


    close(fd);
    freeaddrinfo(addrs);
    return 0;
}

