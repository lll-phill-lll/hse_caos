#include <iostream>

int main() {
    for (int i = 0; i < 10000; ++i)
        for(int j = 0; j < 1000; ++j)
            std::cout << "hello world";
}
