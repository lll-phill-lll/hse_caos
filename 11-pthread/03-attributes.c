pthread_attr_t attr;
pthread_attr_init(&attr);

// 1mb stack
pthread_attr_setstacksize(&attr, 1024 * 1024);

// detached thread
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

pthread_t thread;
pthread_create(&thread, &attr, worker, NULL);

pthread_attr_destroy(&attr);
