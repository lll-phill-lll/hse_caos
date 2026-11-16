#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

struct someSharedData {
    int value;
    pid_t writer_pid;
};

int main() {
    int fd = shm_open("/some_file", O_RDONLY, 0);

    struct someSharedData *data = mmap(
        NULL, sizeof(*data), PROT_READ, MAP_SHARED, fd, 0);
    close(fd);

    printf("value = %d, writer_pid = %ld\n",
        data->value, (long)data->writer_pid);

    munmap(data, sizeof(*data));
    shm_unlink("/some_file");
}
