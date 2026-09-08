#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8088
#define BUF_SIZE 4096

int main() {
    const char *host = "127.0.0.1";

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
    };
    inet_pton(AF_INET, host, &addr.sin_addr);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        return 1;
    }
    printf("Connected to %s:%d\n", host, PORT);

    const char *msg = "hello";
    write(fd, msg, strlen(msg));
    printf("Sent: %s\n", msg);

    char buf[BUF_SIZE];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("Received: %s\n", buf);
    }

    close(fd);
    printf("Done\n");
    return 0;
}
