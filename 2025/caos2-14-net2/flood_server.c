#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <signal.h>

#define PORT 8088
#define BUF_SIZE 128

typedef struct {
    uint32_t seq;
    char payload[60];
} Packet;

double now_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

void run_tcp() {
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
    printf("[TCP] waiting for client on :%d ...\n", PORT);

    int client_fd = accept(server_fd, NULL, NULL);
    printf("[TCP] client connected, receiving packets...\n");

    Packet pkt;
    uint64_t count = 0;
    uint64_t bytes = 0;
    uint32_t max_seq = 0;
    double t_start = now_sec();

    for (;;) {
        ssize_t n = read(client_fd, &pkt, sizeof(pkt));
        if (n <= 0) break;
        count++;
        bytes += n;
        if (pkt.seq > max_seq)
            max_seq = pkt.seq;
    }

    double elapsed = now_sec() - t_start;
    printf("\n=== TCP Results (server) ===\n");
    printf("Packets received:  %lu\n", (unsigned long)count);
    printf("Max seq seen:      %u\n", max_seq);
    printf("Bytes received:    %lu\n", (unsigned long)bytes);
    printf("Time:              %.3f sec\n", elapsed);
    printf("Packets/sec:       %.0f\n", count / elapsed);
    printf("Throughput:        %.2f MB/s\n", bytes / elapsed / 1024 / 1024);

    close(client_fd);
    close(server_fd);
}

void run_udp() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };
    bind(fd, (struct sockaddr *)&addr, sizeof(addr));

    printf("[UDP] waiting for packets on :%d ...\n", PORT);
    printf("[UDP] will stop 2 seconds after last packet\n");

    Packet pkt;
    uint64_t count = 0;
    uint64_t bytes = 0;
    uint32_t max_seq = 0;
    uint32_t out_of_order = 0;
    uint32_t last_seq = 0;
    double t_start = 0;

    struct timeval tv = { .tv_sec = 2, .tv_usec = 0 };
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    int started = 0;
    for (;;) {
        ssize_t n = recvfrom(fd, &pkt, sizeof(pkt), 0, NULL, NULL);
        if (n < 0) {
            if (started) break;
            continue;
        }
        if (!started) {
            started = 1;
            t_start = now_sec();
            printf("[UDP] first packet received, counting...\n");
        }
        count++;
        bytes += n;
        uint32_t seq = pkt.seq;
        if (seq > max_seq) max_seq = seq;
        if (seq < last_seq) out_of_order++;
        last_seq = seq;
    }

    double elapsed = now_sec() - t_start;
    uint32_t lost = (max_seq > 0) ? (max_seq - count) : 0;

    printf("\n=== UDP Results (server) ===\n");
    printf("Packets received:  %lu\n", (unsigned long)count);
    printf("Packets sent (seq):%u\n", max_seq);
    printf("Packets lost:      %u (%.1f%%)\n", lost,
           max_seq > 0 ? lost * 100.0 / max_seq : 0.0);
    printf("Out-of-order:      %u\n", out_of_order);
    printf("Bytes received:    %lu\n", (unsigned long)bytes);
    printf("Time:              %.3f sec\n", elapsed);
    printf("Packets/sec:       %.0f\n", count / elapsed);
    printf("Throughput:        %.2f MB/s\n", bytes / elapsed / 1e6);

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 2 || (strcmp(argv[1], "tcp") && strcmp(argv[1], "udp"))) {
        fprintf(stderr, "Usage: %s <tcp|udp>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "tcp") == 0)
        run_tcp();
    else
        run_udp();

    return 0;
}
