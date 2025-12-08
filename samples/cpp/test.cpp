#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>

std::ostream& operator<<(std::ostream& os, const std::unordered_map<int, std::vector<int>>& m) {
    os << "{" << std::endl;
    for (const auto& pair: m) {
        os << pair.first << ": [";
        for (int i = 0; i < pair.second.size(); i++) {
            os << pair.second[i];
            if (i != pair.second.size()-1) os << ",";
        }
        os << "]" << std::endl;
    }
    os << "}" << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, std::vector<int>& v) {
    os << "[";
    for (int i = 0; i < v.size(); i++) {
        os << v[i];
        if (i != v.size()-1) os << ",";
    }
    os << "]" << std::endl;
    return os;
}

int main() {
    return 0;
}