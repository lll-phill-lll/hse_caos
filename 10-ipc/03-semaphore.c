

// ------ WRITER ------
sem_t *ready = sem_open("/ipc-ready", O_CREAT | O_EXCL, 0600, 0u);

data->value = 42;
data->writer_pid = getpid();

sem_post(ready);
sem_close(ready);

// ----- READER ----- 
sem_t *ready = sem_open("/ipc-ready", 0);

sem_wait(ready);
printf("value = %d, writer_pid = %ld\n",
    data->value, (long)data->writer_pid);

sem_close(ready);
sem_unlink("/ipc-ready");
