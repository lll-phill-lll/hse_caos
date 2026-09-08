#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    std::string cmd;

    while (true) {
        std::cout << "\033[1;33mEnter command\033[0m$ " << std::endl;

        std::cin >> cmd;

        pid_t pid = fork();
        if (pid == 0) {
            execlp(cmd.c_str(), cmd.c_str(), nullptr);
            std::perror("exec");
            _exit(1);
        }

        int status = 0;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            if (exit_status != 0) {
                std::cout << "Non zero exit status" << std::endl;
            }
        } else if (WIFSIGNALED(status)) {
            std::cout << "Terminated by signal: " << WTERMSIG(status) << std::endl;
        }
    }
}

