#include <iostream>

class Base {
public:
    bool operator==(const Base& b) const {
        return true;
    }
    bool operator!=(const Base& b) const {
        return false;
    }
    Base& operator=(Base& b) = delete;
};


int main() {
    Base b0;
    Base b1;
    std::cout << (b0 == b1) << std::endl;
    // b0 = b1; // Compile fail on deleted operator
    return 0;
};
