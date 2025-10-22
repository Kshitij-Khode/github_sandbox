#include <iostream>

void exceptionThrowingFun() {};

void noExceptionThrowingFun() noexcept {};

template<typename T>
void conditionalExceptionThrowingFun(T t) noexcept(std::is_integral_v<T>) {
};

int main() {
    std::cout << noexcept(exceptionThrowingFun()) << std::endl; // False. Will throw.
    std::cout << noexcept(noExceptionThrowingFun()) << std::endl; // True. Will not throw.
    std::cout << noexcept(conditionalExceptionThrowingFun(1)) << std::endl; // True. Will not throw.
    std::cout << noexcept(conditionalExceptionThrowingFun(2.0)) << std::endl; // False. Will throw.
    return 0;
};
