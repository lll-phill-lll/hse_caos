#include <iostream>

extern "C"
int multiply_add(int, int);

int main() {
    int A = 111;
    int B = 2000;

    int res = multiply_add(A, B);
    std::cerr << "Number: " << res << std::endl;
}
