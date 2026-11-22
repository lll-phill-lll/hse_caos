#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define SEC 3

double now() {
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
        perror("clock_gettime");
        exit(1);
    }

    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void* thread_function(void* arg) {
    return NULL;
}

void process_function() {
    _exit(0);
}

void measure_threads() {
    int count = 0;
    double start = now();

    while (now() - start < SEC) {
        pthread_t thread;

        int rc = pthread_create(&thread, NULL, thread_function, NULL);
        if (rc != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(rc));
            exit(1);
        }

        rc = pthread_join(thread, NULL);
        if (rc != 0) {
            fprintf(stderr, "pthread_join: %s\n", strerror(rc));
            exit(1);
        }

        count++;
    }

    double elapsed = now() - start;
    printf("Threads per second: %.0f (%d total)\n",
           count / elapsed, count);
}

void measure_processes() {
    int count = 0;
    double start = now();

    while (now() - start < SEC) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(1);
        }

        if (pid == 0) {
            process_function();
        }

        int status;
        pid_t result;

        do {
            result = waitpid(pid, &status, 0);
        } while (result == -1 && errno == EINTR);

        if (result == -1) {
            perror("waitpid");
            exit(1);
        }

        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Child %ld did not exit successfully\n",
                    (long)pid);
            exit(1);
        }

        count++;
    }

    double elapsed = now() - start;
    printf("Processes per second: %.0f (%d total)\n",
           count / elapsed, count);
}

int main() {
    printf("Measuring thread creation speed...\n");
    measure_threads();

    printf("Measuring process creation speed...\n");
    measure_processes();

    return 0;
}
