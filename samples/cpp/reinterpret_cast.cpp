#include <iostream>

int main() {
    long long int test = 0xFFFFFFFF;
    void* testp = reinterpret_cast<void*>(test);
    return 0;
};
