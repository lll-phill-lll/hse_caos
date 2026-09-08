#include <iostream>
#include <thread>
#include <future>

// Получаем значение из потока
void func(std::promise<int> p) {
    // записываем в promise
    // p.set_value(123);
}

int main() {
    // создаем пару promise->future. В promise поток что-то запишет, из future другой поток это
    // прочитает
    std::promise<int> p;
    std::future<int> f = p.get_future();


    // th1  p.set_value(55);
    // th2 int res = f.get();

    // promise и future нельзя копировать (не скомпилируется, так как copy конструктор и оператор
    // присваиваниня удалены)
    // std::thread thr(func, std::move(p));

    // // выплонение зависнет в этом вызове до тех пор, пока другой поток не запишет что-то в
    // // соответствующий promise
    // // Чтобы не зависать, можно использовать wait_for, wait_until, которые сколько-то подождут.
    // // И можно поделать что-то полезное, если за время ожидания, никто не написал в promise.
    int res = f.get();
    thr.join();



    std::cout << res << std::endl;

    // исключение. Записать в promise и прочитать из future можно только один раз.
    // res = f.get();
}
