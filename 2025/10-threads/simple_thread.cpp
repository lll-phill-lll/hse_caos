#include <iostream>
#include <thread>

// можно передавать несколько аргументов. Их все перечисляем при создании std::thread
// в конструкторе
// возвращаемое значение функции игнорируется
void thread_func(int i) {
    std::cout << "hello world " << i << std::endl;
}

int main(){
    std::thread thr(thread_func, 2);
    // join делать обязательно, чтобы не дожидаться результата, можно сделать
    // thr.detach();
    thr.join();


    // конструктор копирования у тредов удален. Копировать потоки нельзя.
    // std::thread thr2 = thr;

    // можно сделать move и передать владение другому объекту
    // std::thread thr2(std::move(thr));
    // Но в случае с move надо будет делать join уже от thr2, thr больше использовать нельзя
    // thr2.join();
}
