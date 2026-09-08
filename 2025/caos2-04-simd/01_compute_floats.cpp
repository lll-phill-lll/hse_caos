#include <iostream>

float process(float a, float b, int c, float d) {
    return (a + b) / ((float)c + d);
}

// extern "C"
// float process(float a, float b, int c, float d);
// 

// FPU x87

int main() {
    float a = 1.1;
    float b = 2.2;
    int c = 3;
    float d = 4.4;

    float res = process(a, b, c, d);

    std::cout << res << std::endl;
}
