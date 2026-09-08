#include <iostream>
#include <string>
#include <array>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <format>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << std::format("Usage: {} NODE SERVICE", argv[0]) << std::endl;
        return 1;
    }

    const std::string node    = argv[1];
    const std::string service = argv[2];

    addrinfo hints{};
    // hints.ai_family = AF_INET6;

    addrinfo* res = nullptr;
    int gai_err = getaddrinfo(node.c_str(), service.c_str(), &hints, &res);
    if (gai_err != 0) {
        std::cerr << std::format("gai error: {}", gai_strerror(gai_err)) << std::endl;
        return 2;
    }

    for (addrinfo* ai = res; ai != nullptr; ai = ai->ai_next) {
        std::array<char, 1024> buf{};

        std::cout << std::format(
            "ai_flags={}, ai_family={}, ai_socktype={}",
            ai->ai_flags, ai->ai_family, ai->ai_socktype
        ) << std::endl;

        switch (ai->ai_family) {
            case AF_INET: {
                auto* inet = reinterpret_cast<sockaddr_in*>(ai->ai_addr);

                // 127.0.0.1
                // a::rr:42
                // 32
                const char* s = inet_ntop(
                    ai->ai_family,
                    &inet->sin_addr,
                    buf.data(), buf.size()
                );

                if (!s) {
                    std::cerr << "inet_ntop failed" << std::endl;
                    break;
                }

                std::cout << std::format(
                    "\taddress: {}, port: {}",
                    buf.data(),
                    // n -net 
                    // to ->
                    // h - host
                    // s - short
                    ntohs(inet->sin_port)
                ) << std::endl;

                break;
            }

            case AF_INET6: {
                auto* inet6 = reinterpret_cast<sockaddr_in6*>(ai->ai_addr);

                const char* s = inet_ntop(
                    ai->ai_family,
                    &inet6->sin6_addr,
                    buf.data(), buf.size()
                );

                if (!s) {
                    std::cerr << "inet_ntop failed" << std::endl;
                    break;
                }

                std::cout << std::format(
                    "\taddress: {}, port: {}",
                    buf.data(),
                    ntohs(inet6->sin6_port)
                ) << std::endl;

                break;
            }

            default:
                std::cout << "\tunknown address family" << std::endl;
                break;
        }
    }

    freeaddrinfo(res);

    return 0;
}

