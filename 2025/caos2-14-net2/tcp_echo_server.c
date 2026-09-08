#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8088
#define BUF_SIZE 4096

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };
    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 1);
    printf("Server listening on :%d\n", PORT);

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

    char client_ip[INET_ADDRSTRLEN];
    // 32 -> 127.0.0.1
    // pton
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    printf("Client connected: %s:%d\n", client_ip, ntohs(client_addr.sin_port));

    char buf[BUF_SIZE];
    ssize_t n = read(client_fd, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("Received: %s\n", buf);
    }

    const char *reply = "how are you?";
    write(client_fd, reply, strlen(reply));
    printf("Sent: %s\n", reply);

    close(client_fd);
    close(server_fd);
    printf("Done\n");
    return 0;
}
