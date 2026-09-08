#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>

int main() {
    const char *path = "output.txt";
    int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) { perror("open"); return 1; }
    // getchar();

    char hello[] = "hello ";
    write(fd, hello, sizeof(hello));

    int fd_dup = dup(fd);
    if (fd_dup == -1) { perror("dup"); return 1; }

    char world[] = "world\n";
    write(fd_dup, world, sizeof(world));

    getchar();
    dup2(fd, STDOUT_FILENO);
    getchar();
    std::cout << "I WAS PRINTED TO STDOUT" << std::endl;

    close(fd);
    close(fd_dup);
    return 0;
}
