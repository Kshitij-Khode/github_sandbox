#include <iostream>

class Base {
public:
    friend class Friend;
private:
    int i = 9;
};

class Friend {
public:
    void modify(Base& b) {
        b.i = 10;
        std::cout << b.i << std::endl;
    }
};

int main() {
    Friend f;
    Base b;
    f.modify(b);
    return 0;
};
