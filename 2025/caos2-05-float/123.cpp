#include <iostream>
#include <iomanip>
#include <limits>

int main() {
    double x = 0.1 + 0.2;

    std::cout << std::setprecision(std::numeric_limits<double>::max_digits10);

    std::cout << x << std::endl;
}
