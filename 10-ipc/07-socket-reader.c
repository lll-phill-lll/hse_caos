#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in local = {
        .sin_family = AF_INET,
        .sin_port = htons(9000),
        .sin_addr.s_addr = htonl(INADDR_LOOPBACK),
    };
    bind(fd, (struct sockaddr *)&local, sizeof(local));

    char message[64] = {0};
    read(fd, message, sizeof(message) - 1);
    puts(message);
    close(fd);
}
