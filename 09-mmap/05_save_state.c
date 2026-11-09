#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

struct State {
    int launches;
    int value;
};

int main() {
    int fd = open("state.bin", O_RDWR | O_CREAT, 0644);

    struct stat st;
    fstat(fd, &st);

    if (st.st_size == 0) {
        ftruncate(fd, sizeof(struct State));
    }

    struct State *state = mmap(
        NULL, sizeof(struct State),
        PROT_READ | PROT_WRITE,
        MAP_SHARED, fd, 0
    );
    close(fd);

    printf("Launches: %d\n", state->launches);
    printf("Value: %d\n", state->value);
    // printf("ptr: %p\n", (void*)state);

    state->launches++;
    state->value += 10;

    msync(state, sizeof(struct State), MS_SYNC);
    munmap(state, sizeof(struct State));

    return 0;
}
