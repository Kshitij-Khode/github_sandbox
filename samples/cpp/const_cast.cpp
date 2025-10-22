#include <iostream>

int main(int argc, char** argv) {
    const int& p = 5;
    const_cast<int&>(p) = 10;
    std::cout << p << std::endl;
    return 0;
}
