#include <iostream>
#include <generator>

std::generator<int> count(int count) {
    for (int i = 0; i < count; i++) {
        co_yield i;
    }
    co_return;
}

int main() {
    for (int i: count(5)) {
        std::cout << i << std::endl;
    }
    return 0;
}
