#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s NODE SERVICE\n", argv[0]);
        return 1;
    }
    struct addrinfo *res = NULL;
    struct addrinfo hints = {};
    
    hints.ai_family = AF_INET;
    int gai_err;
    // ya.ru http 
    if ((gai_err = getaddrinfo(argv[1], argv[2], &hints, &res))) {
        fprintf(stderr, "gai error: %s\n", gai_strerror(gai_err));
        return 2;
    }
    for (struct addrinfo *ai = res; ai; ai = ai->ai_next) {
        char buf[1024];
        printf("ai_flags=%d, ai_family=%d, ai_socktype=%d\n",
                ai->ai_flags, ai->ai_family, ai->ai_socktype);

        struct sockaddr_in *inet;
        struct sockaddr_in6 *inet6;
        switch (ai->ai_family) {
            case AF_INET:
                inet = (struct sockaddr_in *)ai->ai_addr;
                printf("\taddress: %s, port: %hu\n",
                    inet_ntop(ai->ai_family, &inet->sin_addr,
                        buf, sizeof(buf)),
                    ntohs(inet->sin_port));
                // net to host short
                break;
            case AF_INET6:
                inet6 = (struct sockaddr_in6 *)ai->ai_addr;
                printf("\taddress: %s, port: %hu\n",
                    inet_ntop(ai->ai_family, &inet6->sin6_addr,
                        buf, sizeof(buf)),
                    ntohs(inet6->sin6_port));
                break;
            default:
                printf("\tunknown address family\n");
        }
    }
    freeaddrinfo(res);
}
