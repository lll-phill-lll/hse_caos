#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#define PORT 8080
#define BACKLOG 16
#define BUF_SIZE 4096

static const char RESPONSE[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/plain\r\n"
    "Connection: close\r\n"
    "\r\n"
    "Hello from Docker container!\n"
    "PID: %d\n"
    "Hostname: %s\n";

int main(void) {
    signal(SIGPIPE, SIG_IGN);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT),
    };

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        return 1;
    }

    printf("Listening on port %d...\n", PORT);
    fflush(stdout);

    char buf[BUF_SIZE];
    char hostname[256];
    gethostname(hostname, sizeof(hostname));

    for (;;) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        read(client_fd, buf, sizeof(buf));

        int len = snprintf(buf, sizeof(buf), RESPONSE, getpid(), hostname);
        write(client_fd, buf, len);

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
