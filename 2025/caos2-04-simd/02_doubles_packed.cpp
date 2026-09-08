#include <iostream>

extern "C"
void process(double*, double*);
int main() {

    double res1, res2;
    process(&res1, &res2);
    std::cout << res1 << " " << res2 << std::endl;

}
