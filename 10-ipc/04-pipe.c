
int ends[2];
pipe(ends);

if (fork() == 0) {
    close(ends[0]);
    struct result data = {.value = 42, .writer_pid = getpid()};
    write(ends[1], &data, sizeof(data));
    close(ends[1]);
    _exit(0);
}

close(ends[1]);
struct result data;
read_result(ends[0], &data);
close(ends[0]);
printf("value = %d, writer_pid = %d\n", data.value, data.writer_pid);
