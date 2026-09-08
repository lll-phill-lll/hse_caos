#include <cstdio>
#include <vector>

int main() {
    {
        std::vector<int> vec(1 << 20, 5);
    }
    std::vector<int> vec(1 << 20, 5);
    char* buf = new char[10000];
    // delete[] buf;

    void* ptr = std::malloc(10000);
    // std::free(ptr);
}
