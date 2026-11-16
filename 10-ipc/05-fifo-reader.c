#include <fcntl.h>
#include <unistd.h>

struct result {
    int value;
    pid_t writer_pid;
};

int main() {
    int fd = open("fifo", O_WRONLY);
    struct result data = {.value = 42, .writer_pid = getpid()};

    write(fd, &data, sizeof(data));
    close(fd);
}
