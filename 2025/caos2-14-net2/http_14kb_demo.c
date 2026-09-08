#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define PORT 8088
#define MAX_RESPONSE (4 * 1024 * 1024)

int parse_size(const char *s) {
    int val = atoi(s);
    if (val <= 0) return 0;
    const char *p = s;
    while (*p >= '0' && *p <= '9') p++;
    if (*p == 'K' || *p == 'k') val *= 1024;
    else if (*p == 'M' || *p == 'm') val *= 1024 * 1024;
    return val;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <response_size>\n", argv[0]);
        fprintf(stderr, "Examples: %s 5KB, %s 14KB, %s 50KB, %s 200KB\n",
                argv[0], argv[0], argv[0], argv[0]);
        return 1;
    }

    int body_size = parse_size(argv[1]);
    if (body_size <= 0 || body_size > MAX_RESPONSE) {
        fprintf(stderr, "Invalid size (1 byte .. 4 MB)\n");
        return 1;
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };
    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 16);

    char header[256];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n", body_size);

    int total_size = header_len + body_size;
    char *response = malloc(total_size);
    memcpy(response, header, header_len);

    char *body = response + header_len;
    const char *line = "<p>AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA</p>\n";
    int line_len = strlen(line);
    for (int i = 0; i < body_size; i++)
        body[i] = line[i % line_len];

    printf("=== HTTP 14 KB Demo ===\n");
    printf("Listening on        :%d\n", PORT);
    printf("Response body size: %d bytes (%.1f KB)\n", body_size, body_size / 1024.0);
    printf("Total response:     %d bytes (%.1f KB)\n", total_size, total_size / 1024.0);
    printf("initcwnd (typical): 10 segments × 1460 = 14600 bytes (14.3 KB)\n");
    printf("\n");

    if (total_size <= 14600)
        printf(">>> Response FITS in initcwnd → expect 1 burst, 1 RTT after handshake\n");
    else
        printf(">>> Response EXCEEDS initcwnd → expect multiple bursts, multiple RTTs\n");

    printf("\nWaiting for connections... (Ctrl+C to stop)\n");

    for (;;) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;

        char req[4096];
        read(client_fd, req, sizeof(req));

        write(client_fd, response, total_size);

        close(client_fd);
        printf("  served %d bytes (%.1f KB)\n", total_size, total_size / 1024.0);
    }

    free(response);
    close(server_fd);
    return 0;
}
