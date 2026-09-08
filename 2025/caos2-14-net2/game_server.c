#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <time.h>

#define PORT 9400

typedef struct {
    uint32_t seq;
    float x;
    float y;
} __attribute__((packed)) GamePacket;

void stream(int fd, struct sockaddr_in *dest, int hz) {
    long interval_us = 1000000L / hz;
    GamePacket pkt;
    uint32_t seq = 0;
    struct timespec ts;

    for (;;) {
        clock_gettime(CLOCK_MONOTONIC, &ts);
        double t = ts.tv_sec + ts.tv_nsec * 1e-9;

        seq++;
        pkt.seq = seq;
        pkt.x = 0.5f + 0.4f * sinf(3.0f * t);
        pkt.y = 0.5f + 0.4f * sinf(2.0f * t);

        ssize_t ret;
        if (dest)
            ret = sendto(fd, &pkt, sizeof(pkt), 0,
                         (struct sockaddr *)dest, sizeof(*dest));
        else
            ret = write(fd, &pkt, sizeof(pkt));

        if (ret <= 0) break;
        usleep(interval_us);
    }
}

int main(int argc, char *argv[]) {
    int is_tcp = 0;
    int hz = 30;

    if (argc >= 2 && strcmp(argv[1], "tcp") == 0)
        is_tcp = 1;
    else if (argc >= 2 && strcmp(argv[1], "udp") == 0)
        is_tcp = 0;
    else {
        fprintf(stderr, "Usage: %s <udp|tcp> [hz=30]\n", argv[0]);
        return 1;
    }
    if (argc >= 3) {
        hz = atoi(argv[2]);
        if (hz <= 0) hz = 30;
    }

    int server_fd = socket(AF_INET,
                           is_tcp ? SOCK_STREAM : SOCK_DGRAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };
    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));

    if (is_tcp) {
        listen(server_fd, 1);
        printf("[TCP] Waiting for client on :%d (%d Hz)\n", PORT, hz);
        int client_fd = accept(server_fd, NULL, NULL);
        int flag = 1;
        setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
        printf("[TCP] Client connected, streaming...\n");
        stream(client_fd, NULL, hz);
        printf("[TCP] Client disconnected\n");
        close(client_fd);
    } else {
        printf("[UDP] Waiting for client on :%d (%d Hz)\n", PORT, hz);
        struct sockaddr_in client;
        socklen_t clen = sizeof(client);
        char hello[16];
        recvfrom(server_fd, hello, sizeof(hello), 0,
                 (struct sockaddr *)&client, &clen);
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client.sin_addr, ip, sizeof(ip));
        printf("[UDP] Client %s:%d connected, streaming...\n",
               ip, ntohs(client.sin_port));
        stream(server_fd, &client, hz);
    }

    close(server_fd);
    return 0;
}
