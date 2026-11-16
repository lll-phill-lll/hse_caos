#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

struct result {
    int value;
    pid_t writer_pid;
};

int main() {
    const char *path = "fifo";
    mkfifo(path, 0600);

    int fd = open(path, O_RDONLY);
    struct result data = {0};
    read(fd, &data, sizeof(data));

    printf("value = %d, writer_pid = %d\n",
           data.value, data.writer_pid);

    close(fd);
    unlink(path);
}
