#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in destination = {
        .sin_family = AF_INET,
        .sin_port = htons(9000),
    };
    inet_pton(AF_INET, "127.0.0.1", &destination.sin_addr);
    connect(fd, (struct sockaddr *)&destination, sizeof(destination));

    char message[64];
    int length = snprintf(message, sizeof(message),
                          "value = 42, writer_pid = %ld", (long)getpid());
    write(fd, message, (size_t)length);
    close(fd);
}
