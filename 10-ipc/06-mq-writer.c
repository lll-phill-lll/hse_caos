
#include <fcntl.h>
#include <mqueue.h>
#include <unistd.h>


int main() {
    mqd_t queue = mq_open("/queue", O_WRONLY);
    struct result data = {.value = 42, .writer_pid = getpid()};

    mq_send(queue, (const char *)&data, sizeof(data), 0);
    mq_close(queue);
}
