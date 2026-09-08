#include <iostream>
#include <thread>
#include <future>

// теперь уже возвращаемое значение функции не игнорируется
// Это возвращаемое значение потом можно получить из future
int func(int i) {
    return 123 + i;
}

int main() {
    // async позволяет немного приятнее работать с механизмом promise/future
    //
    // Правила передачи аргументов (про копирования и ссылки) такие же, как и в std::thread
    // Тип в шаблоне future должен совпадать с типом возвращаемого значения из вызываемой функции
    std::future<int> f = std::async(func, 1);

    // Также можно указать политику запуска: async или deferred.
    // Async не отличается от обычного запуска потока. deferred функция будет запущена лениво,
    // то есть в тот момент, когда ее результат понадобится (то есть вызовут .get() или .wait())
    // std::future<int> f = std::async(std::launch::deferred, func, 1);

    int res;
    res = f.get();

    std::cout << res << std::endl;
}
