#include <iostream>
#include <vector>
#include <unordered_map>

std::vector<int> indexDist(std::vector<int>& input) {
    std::unordered_map<int, std::vector<int>> indexMap;
    for (int i = 0; i < input.size(); i++) {
        indexMap[input[i]].push_back(i);
    }

    std::vector<int> ret;
    for (int i = 0; i < input.size(); i ++) {
        std::vector<int> indexes = indexMap[input[i]];
        int dist = 0;
        for (int ind: indexes) {
            if (ind == i) continue;
            dist += std::abs(i-ind);
        }
        ret.push_back(dist);
    }
    return ret;
}

int main() {
    std::vector<int> ip = {1,2,3,1,5,3,1,4,4};
    std::vector<int> op = indexDist(ip);
    for (int d: op) {
        std::cout << d << std::endl;
    }
}