#include <iostream>
#include <thread>
#include <chrono>

void func() {
    std::cout << "hello" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

int main() {
    int count = 0;
    while(1) {
        try {
            std::thread thread(func);
            thread.detach();
            ++count;
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
            std::cout << count << std::endl;
            break;
        }
    }
}

