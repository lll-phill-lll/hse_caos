#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <time.h>

#define PORT 9400
#define TRAIL_LEN 40
#define MAX_W 200
#define MAX_H 60

typedef struct {
    uint32_t seq;
    float x;
    float y;
} __attribute__((packed)) GamePacket;

static int trail_x[TRAIL_LEN], trail_y[TRAIL_LEN];
static int trail_idx = 0, trail_count = 0;

double now_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

void get_term_size(int *cols, int *rows) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
    } else {
        *cols = 80;
        *rows = 24;
    }
}

int setup_connection(int is_tcp, const char *host) {
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
    };
    inet_pton(AF_INET, host, &addr.sin_addr);

    if (is_tcp) {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        printf("Connecting to %s:%d (TCP)...\n", host, PORT);
        if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("connect");
            exit(1);
        }
        struct timeval tv = { .tv_sec = 0, .tv_usec = 50000 };
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        return fd;
    }

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    printf("Registering with %s:%d (UDP)...\n", host, PORT);
    sendto(fd, "hi", 2, 0, (struct sockaddr *)&addr, sizeof(addr));

    struct timeval tv = { .tv_sec = 0, .tv_usec = 50000 };
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    return fd;
}

ssize_t recv_packet(int fd, int is_tcp, GamePacket *pkt) {
    if (!is_tcp)
        return recvfrom(fd, pkt, sizeof(*pkt), 0, NULL, NULL);

    size_t got = 0;
    while (got < sizeof(*pkt)) {
        ssize_t n = read(fd, (char *)pkt + got, sizeof(*pkt) - got);
        if (n <= 0) return n;
        got += n;
    }
    return got;
}

int main(int argc, char *argv[]) {
    if (argc < 2 || (strcmp(argv[1], "udp") && strcmp(argv[1], "tcp"))) {
        fprintf(stderr, "Usage: %s <udp|tcp> [server_ip=127.0.0.1]\n", argv[0]);
        return 1;
    }

    int is_tcp = (strcmp(argv[1], "tcp") == 0);
    const char *host = (argc >= 3) ? argv[2] : "127.0.0.1";

    int fd = setup_connection(is_tcp, host);

    int cols, rows;
    get_term_size(&cols, &rows);
    int draw_w = cols - 2;
    int draw_h = rows - 4;
    if (draw_w > MAX_W) draw_w = MAX_W;
    if (draw_h > MAX_H) draw_h = MAX_H;

    uint32_t received = 0, max_seq = 0;
    int cur_px = draw_w / 2, cur_py = draw_h / 2;

    double fps_time = now_sec();
    uint32_t fps_frames = 0;
    double fps = 0;

    int buf_size = (draw_w + 3) * (draw_h + 3) * 16 + 512;
    char *buf = malloc(buf_size);

    printf("\033[2J\033[?25l");
    fflush(stdout);

    GamePacket pkt;
    for (;;) {
        ssize_t n = recv_packet(fd, is_tcp, &pkt);
        if (n != sizeof(pkt))
            continue;

        uint32_t seq = pkt.seq;
        received++;
        fps_frames++;
        if (seq > max_seq) max_seq = seq;

        cur_px = 1 + (int)(pkt.x * draw_w);
        cur_py = 1 + (int)(pkt.y * draw_h);
        if (cur_px < 1) cur_px = 1; if (cur_px > draw_w) cur_px = draw_w;
        if (cur_py < 1) cur_py = 1; if (cur_py > draw_h) cur_py = draw_h;

        trail_x[trail_idx] = cur_px;
        trail_y[trail_idx] = cur_py;
        trail_idx = (trail_idx + 1) % TRAIL_LEN;
        if (trail_count < TRAIL_LEN) trail_count++;

        double now = now_sec();
        if (now - fps_time >= 1.0) {
            fps = fps_frames / (now - fps_time);
            fps_frames = 0;
            fps_time = now;
        }

        char *p = buf;
        p += sprintf(p, "\033[H");

        for (int r = 0; r <= draw_h + 1; r++) {
            for (int c = 0; c <= draw_w + 1; c++) {
                if (r == 0 || r == draw_h + 1 || c == 0 || c == draw_w + 1) {
                    *p++ = '+';
                } else if (c == cur_px && r == cur_py) {
                    p += sprintf(p, "\033[1;32m@\033[0m");
                } else {
                    int is_trail = 0;
                    for (int t = 0; t < trail_count; t++) {
                        int ti = (trail_idx - 1 - t + TRAIL_LEN) % TRAIL_LEN;
                        if (trail_x[ti] == c && trail_y[ti] == r) {
                            is_trail = 1;
                            break;
                        }
                    }
                    if (is_trail)
                        p += sprintf(p, "\033[2;32m.\033[0m");
                    else
                        *p++ = ' ';
                }
            }
            *p++ = '\n';
        }

        uint32_t lost = max_seq - received;
        double loss_pct = max_seq > 0 ? lost * 100.0 / max_seq : 0;
        p += sprintf(p,
            " [%s]  FPS: %-5.1f  seq=%-8u  recv=%-8u  lost=%-6u (%.1f%%)",
            is_tcp ? "TCP" : "UDP",
            fps, seq, received, lost, loss_pct);

        write(STDOUT_FILENO, buf, p - buf);
    }

    free(buf);
    printf("\033[?25h\n");
    close(fd);
    return 0;
}
