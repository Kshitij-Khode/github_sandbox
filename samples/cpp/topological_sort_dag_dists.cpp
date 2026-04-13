#include <vector>
#include <deque>
#include <unordered_map>
#include <iostream>
#include <optional>

using namespace std;

optional<tuple<int, int>> dagMinMaxDists(int node, vector<int>& from, vector<tuple<int, int>>& to) {
    if (from.size() != to.size()) return nullopt;

    unordered_map<int, vector<tuple<int, int>>> outgoing;
    unordered_map<int, int> incomingCount;

    int largestNode = INT_MIN;
    for (int i = 0; i < from.size(); i++) {
        auto fromIndex = from[i];
        const auto [toIndex, dist] = to[i];
        if (outgoing.find(fromIndex) == outgoing.end()) outgoing[fromIndex] = vector<tuple<int, int>>();
        if (incomingCount.find(toIndex) == incomingCount.end()) incomingCount[toIndex] = 0;
        outgoing[fromIndex].push_back({toIndex, dist});
        incomingCount[toIndex]++;
        largestNode = std::max(largestNode, fromIndex);
        largestNode = std::max(largestNode, toIndex);
    }

    if (node >= from.size()) return nullopt;

    deque<int> next;
    vector<int> minDist(largestNode+1, INT_MAX);
    vector<int> maxDist(largestNode+1, INT_MIN);
    minDist[node] = 0;
    maxDist[node] = 0;
    for (int i = 0; i < incomingCount.size(); i++) {
        if (incomingCount[i] == 0) next.push_back(i);
    }

    if (next.size() == 0) return nullopt;

    int visits = 0;
    while (next.size() > 0) {
        int cNode = next.front(); next.pop_front();
        visits++;
        for (const auto [nNodeIndex, nNodeDist]: outgoing[cNode]) {
            incomingCount[nNodeIndex]--;
            if (incomingCount[nNodeIndex] == 0) next.push_back(nNodeIndex);
            if (minDist[cNode] != INT_MAX) minDist[nNodeIndex] = std::min(minDist[nNodeIndex], minDist[cNode]+nNodeDist);
            if (maxDist[cNode] != INT_MIN) maxDist[nNodeIndex] = std::max(maxDist[nNodeIndex], maxDist[cNode]+nNodeDist);
        }
    }

    if (visits != outgoing.size()) return nullopt;
    return optional<tuple<int, int>>(make_tuple(
        *(std::min_element(minDist.begin(), minDist.end())),
        *(std::max_element(maxDist.begin(), maxDist.end()))
    ));
}

int main() {
    std::vector<int> go_from = {0, 0, 1, 1, 2, 2, 2, 3, 3, 4};
    std::vector<tuple<int, int>> go_to = {{1, 5}, {2, 3}, {3, 6}, {2, 2}, {4, 4}, {5, 2}, {3, 7}, {5, 1}, {4, -1}, {5, -2}};
    const auto [minDist, maxDist] = *(dagMinMaxDists(1, go_from, go_to));
    // Fix issue with self dist 0 becoming least dist
    std::cout << minDist << " " << maxDist << std::endl;
    return 0;
};
