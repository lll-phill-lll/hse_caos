#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <time.h>

#define PORT 8088
#define DEFAULT_DURATION 5

typedef struct {
    uint32_t seq;
    char payload[60];
} Packet;

double now_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

void run_tcp(const char *host, int duration) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
    };
    inet_pton(AF_INET, host, &addr.sin_addr);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        return;
    }

    int flag = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

    printf("[TCP] connected to %s:%d (TCP_NODELAY=on)\n", host, PORT);
    printf("[TCP] flooding for %d seconds...\n", duration);

    Packet pkt;
    memset(&pkt, 0, sizeof(pkt));
    memset(pkt.payload, 'T', sizeof(pkt.payload));

    uint32_t seq = 0;
    uint64_t bytes = 0;
    double t_start = now_sec();
    double t_end = t_start + duration;

    while (now_sec() < t_end) {
        seq++;
        pkt.seq = seq;
        ssize_t n = write(fd, &pkt, sizeof(pkt));
        if (n <= 0) { perror("write"); break; }
        bytes += n;
    }

    double elapsed = now_sec() - t_start;
    close(fd);

    printf("\n=== TCP Results (client) ===\n");
    printf("Packets sent:      %u\n", seq);
    printf("Bytes sent:        %lu\n", (unsigned long)bytes);
    printf("Time:              %.3f sec\n", elapsed);
    printf("Packets/sec:       %.0f\n", seq / elapsed);
    printf("Throughput:        %.2f MB/s\n", bytes / elapsed / 1024 / 1024);
}

void run_udp(const char *host, int duration) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
    };
    inet_pton(AF_INET, host, &addr.sin_addr);

    printf("[UDP] sending to %s:%d\n", host, PORT);
    printf("[UDP] flooding for %d seconds...\n", duration);

    Packet pkt;
    memset(&pkt, 0, sizeof(pkt));
    memset(pkt.payload, 'U', sizeof(pkt.payload));

    uint32_t seq = 0;
    uint64_t bytes = 0;
    double t_start = now_sec();
    double t_end = t_start + duration;

    while (now_sec() < t_end) {
        seq++;
        pkt.seq = htonl(seq);
        ssize_t n = sendto(fd, &pkt, sizeof(pkt), 0,
                           (struct sockaddr *)&addr, sizeof(addr));
        if (n < 0) { perror("sendto"); break; }
        bytes += n;
    }

    double elapsed = now_sec() - t_start;
    close(fd);

    printf("\n=== UDP Results (client) ===\n");
    printf("Packets sent:      %u\n", seq);
    printf("Bytes sent:        %lu\n", (unsigned long)bytes);
    printf("Time:              %.3f sec\n", elapsed);
    printf("Packets/sec:       %.0f\n", seq / elapsed);
    printf("Throughput:        %.2f MB/s\n", bytes / elapsed / 1e6);
}

int main(int argc, char *argv[]) {
    if (argc < 2 || (strcmp(argv[1], "tcp") && strcmp(argv[1], "udp"))) {
        fprintf(stderr, "Usage: %s <tcp|udp> [host=127.0.0.1] [duration=5]\n", argv[0]);
        return 1;
    }

    const char *host = (argc >= 3) ? argv[2] : "127.0.0.1";
    int duration = (argc >= 4) ? atoi(argv[3]) : DEFAULT_DURATION;
    if (duration <= 0) duration = DEFAULT_DURATION;

    if (strcmp(argv[1], "tcp") == 0)
        run_tcp(host, duration);
    else
        run_udp(host, duration);

    return 0;
}
