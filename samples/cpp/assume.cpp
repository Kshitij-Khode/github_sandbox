#include <iostream>
#include <vector>

int getFirstElement(const std::vector<int>& v) {
    [[assume(!v.empty())]]; // Compiler can optimize away empty checks
    return v[0];
}

void limiter(float* samples, size_t count) {
    [[assume(reinterpret_cast<std::uintptr_t>(samples) % 32 == 0)]];
    [[assume(count > 0)]];
    for (size_t i = 0; i < count; ++i) {
        samples[i] = std::clamp(samples[i], -1.0f, 1.0f);
    }
}

int main() {
    std::vector<int> data = {42, 7, 13};
    std::cout << "First element: " << getFirstElement(data) << std::endl;
    alignas(32) float data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    limiter(data, 8);
    return 0;
}
