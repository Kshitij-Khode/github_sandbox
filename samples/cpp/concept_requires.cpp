#include <iostream>

template<typename T, typename U>
concept Addable = requires(T t, U u) {
    t + u;
};

template<typename T, typename U> requires Addable<T, U>
T sum(T t, U u) {
    return t + u;
}

int main() {
    std::cout << sum(1, 2) << std::endl;
    return 0;
};
