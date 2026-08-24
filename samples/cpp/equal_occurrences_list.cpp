#include <print>
#include <vector>
#include <unordered_map>
#include <string>
#include <ostream>

struct Result {
    int length;   // length of the longest balanced subarray (0 if none)
    int start;    // inclusive start index (-1 if none)
    int end;      // inclusive end   index (-1 if none)
};

Result longestEqualOccurrences(const std::vector<char>& arr, char a, char b) {
    std::unordered_map<int, int> occurrenceList;

    // Sentinel: Before starting diff 0 is already seen since 0 len substr is balanced technically. Needed when whole string is balanced.
    occurrenceList[0] = -1;

    int diff      = 0;
    int bestLen   = 0;
    int bestStart = -1;
    int bestEnd   = -1;

    for (int i = 0; i < static_cast<int>(arr.size()); ++i) {
        if      (arr[i] == a) diff += 1;
        else if (arr[i] == b) diff -= 1;

        auto it = occurrenceList.find(diff);
        // If diff 1 didn't change between 0 and 1 ith index, means 1-0th index substr i.e. p-pr=r is balanced.
        if (it != occurrenceList.end()) {
            int len = i - it->second;
            if (len > bestLen) {
                bestLen   = len;
                bestStart = it->second + 1;
                bestEnd   = i;
            }
        } else {
            // First time seeing this diff — record it
            occurrenceList[diff] = i;
        }
    }

    return { bestLen, bestStart, bestEnd };
}

int main() {
    Result res = longestEqualOccurrences({'p','r','q','p','s','q','p','q','r','p','q'}, 'p', 'q');
    std::print("{}, {}, {}", res.length, res.start, res.end);
    return 0;
}
