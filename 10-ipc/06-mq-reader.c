#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <sys/types.h>

struct result {
    int value;
    pid_t writer_pid;
};

int main() {
    struct mq_attr attr = {
        .mq_maxmsg = 4,
        .mq_msgsize = sizeof(struct result),
    };
    mqd_t queue = mq_open("/queue", O_CREAT | O_EXCL | O_RDONLY,
                          0600, &attr);

    struct result data = {0};
    mq_receive(queue, (char *)&data, sizeof(data), NULL);
    printf("value = %d, writer_pid = %ld\n",
           data.value, (long)data.writer_pid);

    mq_close(queue);
    mq_unlink("/ipc-results");
}
