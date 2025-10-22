#include <iostream>

int main(int argc, char** argv) {
    std::string i = "";
    int j = 9;
    std::cout << std::is_same_v<decltype(i), decltype(j)> << std::endl;
    return 0;
}
