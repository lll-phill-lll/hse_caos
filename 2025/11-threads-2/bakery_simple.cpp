#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std::chrono_literals;

class CvQueue {
public:
    void push(int value) {
        {
            std::lock_guard<std::mutex> lock(m);
            q.push(value);
        }
        cv.notify_one();
        // ^-- делаем уже при unlock
    }

    int wait_and_pop() {
        std::unique_lock<std::mutex> lock(m);

        cv.wait(lock, [&] {
            return !q.empty();
        });
        // unlock
        // sleep
        // lock

        int value = q.front();
        q.pop();
        return value;
    }

private:
    std::queue<int> q;
    std::mutex m;
    std::condition_variable cv;
};


class BusyQueue {
public:
    void push(int value) {
        std::lock_guard<std::mutex> lock(m);
        q.push(value);
    }

    int wait_and_pop() {
        for (;;) {
            std::lock_guard<std::mutex> lock(m);
            if (!q.empty()) {
                int value = q.front();
                q.pop();
                return value;
            }
            
            // std::this_thread::sleep_for(100ms);
            // std::this_thread::yield();
        }
    }

private:
    std::queue<int> q;
    std::mutex m;
};


// CvQueue queue;
BusyQueue queue;

void baker() {
    while (true) {
        int client_id = queue.wait_and_pop();

        if (client_id == -1) {
            std::cout << "[Пекарь] закрываемся\n";
            break;
        }

        std::cout << "[Пекарь] печёт пирожок для клиента " << client_id << "\n";
        std::this_thread::sleep_for(100ms);
        std::cout << "[Пекарь] отдал пирожок клиенту " << client_id << "\n";
    }
}

void customer(int id) {
    for (int i = 0; i < 30; ++i) {
        std::this_thread::sleep_for(550ms);
        std::cout << "[Клиент][" << id << "] пришёл за пирожком\n";
        queue.push(id);
    }
    std::cout << "[Клиент][" << id << "] наелся и ушёл\n";
}

int main() {
    std::thread baker_thread(baker);

    std::vector<std::thread> customers;
    for (int i = 0; i < 4; ++i) {
        customers.emplace_back(customer, i);
    }

    for (auto &t : customers) {
        t.join();
    }

    queue.push(-1);

    baker_thread.join();
}

