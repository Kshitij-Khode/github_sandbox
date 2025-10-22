#include <iostream>

void test(int i) {
    std::cout << "1" << std::endl;
}

int main() {
    void (*testp)(int);
    testp = &test;
    testp(1);
    return 0;
}
