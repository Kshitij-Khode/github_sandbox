#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<std::tuple<int, int, int>> test = {std::make_tuple(1, 2, 3), std::make_tuple(1, 3, 3), std::make_tuple(3, 2, 3)};
    std::sort(test.begin(), test.end(), [](std::tuple<int, int, int> i, std::tuple<int, int, int> j) {
        auto [fi, si, ti] = i;
        auto [fj, sj, tj] = j;
        if (fi < fj) return true;
        return false;
    });
    for (auto p: test) {
        auto [i, j, k] = p;
        std::cout << i << "," << j << "," << k << std::endl;
    }
    return 0;
}
