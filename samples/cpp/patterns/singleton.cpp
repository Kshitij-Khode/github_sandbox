#include <iostream>

class Singleton {
private:
    Singleton() {}
    ~Singleton() {}
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

public:
    static Singleton& Instance() {
        static Singleton instance;
        return instance;
    }
};

int main() {
    const Singleton& s0 = Singleton::Instance();
    const Singleton& s1 = Singleton::Instance();
    if (&s0 == &s1) {
        std::cout << "Same" << std::endl;
    } else {
        std::cout << "Different" << std::endl;
    }
    return 0;
}