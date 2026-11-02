#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_CMD_LEN 1024

int main(void) {
    char cmd[MAX_CMD_LEN];
    int last_status = 0;

    while (1) {
        printf("\033[1;%dmEnter command\033[0m$ ",
               last_status == 0 ? 32 : 31);
        fflush(stdout);

        scanf("%1023s", cmd);

        pid_t pid = fork();

        if (pid == 0) {
            int fd = open("output", O_WRONLY | O_CREAT | O_APPEND, 0644);
            dup2(fd, 1);
            // close(1)
            //
            execlp(cmd, cmd, NULL);
            perror("exec");
            _exit(1);
        }

        int status;

        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            last_status = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            last_status = 128 + WTERMSIG(status);
        }
    }

    return 0;
}
