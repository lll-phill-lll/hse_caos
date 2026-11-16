#include <stdio.h>
#include <sys/inotify.h>
#include <unistd.h>

int main() {
    int fd = inotify_init1(0);
    inotify_add_watch(fd, ".", IN_CLOSE_WRITE);

    puts("Waiting for IN_CLOSE_WRITE...");
    fflush(stdout);

    _Alignas(struct inotify_event) char buffer[4096] = {0};
    read(fd, buffer, sizeof(buffer));

    const struct inotify_event *event = (const struct inotify_event *)buffer;
    printf("IN_CLOSE_WRITE: %s\n", event->name);

    close(fd);
}
