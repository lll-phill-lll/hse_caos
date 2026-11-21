#include <sys/eventfd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/wait.h>

#define CHILD_NUM 5

void do_some_large_task() {
    pid_t my_pid = getpid();
    sleep(my_pid % CHILD_NUM + 1);

    printf("[%d]: done\n", my_pid % CHILD_NUM);
}

void child_action(int efd) {
    while(1) {
        do_some_large_task();
        uint64_t tasks_finished = 1;
        write(efd, &tasks_finished, sizeof(tasks_finished));
        _exit(0);
    }

    _exit(0);
}

int main() {
    int efd = eventfd(0, 0);

    for (int i = 0; i != CHILD_NUM; ++i) {
        pid_t pid = fork();
        // if we are child
        if (!pid) {
            child_action(efd);
        }
    }

    uint64_t tasks_done = 0;;
    uint64_t total_tasks_done = 0;
    while (1) {
        sleep(3);
        read(efd, &tasks_done, sizeof(tasks_done));
        total_tasks_done += tasks_done;
        printf("[PARENT] tasks done: %lu, total tasks done: %lu\n", tasks_done, total_tasks_done);
        if (total_tasks_done == 5) break;
    }

    for (int i = 0; i != CHILD_NUM; ++i) {
        wait(NULL);
    }
}


