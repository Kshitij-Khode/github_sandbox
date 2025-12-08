#include <iostream>
#include <chrono>

int main() {
    int count = 0;
    const int N = 10000000;
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        if (i == 0) [[unlikely]] {
            // Rare branch
            count += 100;
        } else [[likely]] {
            // Common branch
            count += 1;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Count: " << count << std::endl;
    std::cout << "Elapsed: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms" << std::endl;
    return 0;
}