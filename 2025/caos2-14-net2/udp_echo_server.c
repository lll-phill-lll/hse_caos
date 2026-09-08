#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8088
#define BUF_SIZE 4096

int main() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };
    bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    printf("Server listening on :%d\n", PORT);

    struct sockaddr_in client;
    socklen_t clen = sizeof(client);

    char buf[BUF_SIZE];
    ssize_t n = recvfrom(fd, buf, sizeof(buf) - 1, 0,
                         (struct sockaddr *)&client, &clen);
    if (n > 0) {
        buf[n] = '\0';
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client.sin_addr, ip, sizeof(ip));
        printf("Received from %s:%d: %s\n", ip, ntohs(client.sin_port), buf);
    }

    const char *reply = "how are you?";
    sendto(fd, reply, strlen(reply), 0,
           (struct sockaddr *)&client, clen);
    printf("Sent: %s\n", reply);

    close(fd);
    printf("Done\n");
    return 0;
}
