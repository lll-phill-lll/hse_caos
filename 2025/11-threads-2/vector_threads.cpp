#include <iostream>
#include <thread>
#include <future>
#include <vector>

// Создаем несколько потоков и несколько пар future-promise

void func(std::promise<int> p, int i) {
    p.set_value(i + 100);
}

int main() {
    std::vector<std::thread> threads;
    std::vector<std::future<int>> futures;

    for (int i = 0; i != 5; ++i) {
        // после того, как мы из promise получили future и передали promise
        // в поток, он сам нам больше не нужен.
        std::promise<int> p;
        // future нельзя копировать, тут будет вызвана move версия push_back,
        // так как мы передаем rvalue.
        // В общем случае можно написать futures.push_back(std::move(some_future))
        futures.push_back(p.get_future());
        // promise нельзя копировать, поэтому делаем std::move
        threads.push_back(std::thread(func, std::move(p), i));
    }

    for (int i = 0; i != 5; ++i) {
        // get блокирующий, поэтому результаты выполнения мы получим строго по порядку.
        // чтобы этого избежать, можно воспользоваться wait_for/wait_until
        int res = futures[i].get();
        std::cout << res << std::endl;
    }

    // не забываем поджойнить все потоки
    for (int i = 0; i != 5; ++i) {
        threads[i].join();
    }

}
