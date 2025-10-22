#include <iostream>

constexpr int sum_expr(int a, int b) {
    return a + b;
}

consteval int sum_eval(int a, int b) {
    return a + b;
}

int main(int argc, char** argv) {
    int i = 4;
    constexpr int j = 5;
    std::cout << sum_expr(i, j) << std::endl;
    std::cout << sum_eval(j, j) << std::endl;
    return 0;
}
