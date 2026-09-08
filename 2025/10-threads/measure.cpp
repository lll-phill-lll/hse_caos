#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <sys/wait.h>

#define SEC 3

void thread_function() {
}

void process_function() {
    _exit(0);
}

void measure_threads() {
    int count = 0;
    auto start = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(SEC)) {
        std::thread thread(thread_function);
        thread.detach();
        count++;
    }

    std::cout << "Threads created per second: " << count / SEC << std::endl;
}

void measure_processes() {
    int count = 0;
    auto start = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(SEC)) {
        pid_t pid = fork();
        if (pid == 0) {
            process_function();
        } else {
            count++;
        }
    }

    for (int i = 0; i < count; ++i) {
        wait(NULL);
    }

    std::cout << "Processes created per second: " << count / SEC << std::endl;
}

int main() {
    std::cout << "Measuring thread creation speed..." << std::endl;
    measure_threads();

    std::cout << "Measuring process creation speed..." << std::endl;
    measure_processes();

    return 0;
}

