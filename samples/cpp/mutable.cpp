#include <iostream>

class Base {
public:
    int i = 0;
    mutable int j = 0;
};


int main() {
    const Base b;
    b.j = 1;
    std::cout << b.j << std::endl;
    return 0;
};
