#include <iostream>
#include <thread>
#include <vector>
#include <atomic>

int main() {
    int counter = 0;
    std::vector<std::thread> threads;

    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
                ++counter;
        });
    }

    for (auto &t : threads)
        t.join();

}

