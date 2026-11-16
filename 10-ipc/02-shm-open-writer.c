#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

struct someSharedData {
    int value;
    pid_t writer_pid;
};

int main() {
    int fd = shm_open("/some_file", O_CREAT | O_EXCL | O_RDWR, 0600);
    ftruncate(fd, sizeof(struct someSharedData));

    struct someSharedData *data = mmap(
        NULL, sizeof(*data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    data->value = 42;
    data->writer_pid = getpid();

    munmap(data, sizeof(*data));
}
