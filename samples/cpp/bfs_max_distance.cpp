#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <set>

int maxDistance(std::vector<int>& go_from, std::vector<int>& go_to) {
    std::unordered_map<int, std::set<int>> graph;
    for (int i = 0; i < go_from.size(); i++) {
        graph[go_from[i]].insert(go_to[i]);
        graph[go_to[i]].insert(go_from[i]);
    }

    int retMax = 0;

    for (auto it: graph) {
        int cn = it.first;
        std::set<int> cnNeigh = it.second;

        std::queue<int> next;
        std::set<int> visited;
        std::vector<int> dists(graph.size(), -1);
        next.push(cn);
        visited.insert(cn);
        dists[cn] = 0;

        while (!next.empty()) {
            int tn = next.front(); next.pop();
            std::set<int> tnNeigh = graph[tn];
            
            for (int tnn: tnNeigh) {
                if (visited.find(tnn) != visited.end()) continue;        
                next.push(tnn);
                visited.insert(tnn);
                dists[tnn] = dists[tn]+1;
            }
        }

        int currMax = *std::max_element(dists.begin(), dists.end());
        if (currMax > retMax) retMax = currMax;
    }
    return retMax;
}

int main() {
    std::vector<int> go_from = {0, 0, 1, 2, 2, 3, 4, 4};
    std::vector<int> go_to = {1, 2, 3, 3, 4, 5, 5, 0};
    std::cout << maxDistance(go_from, go_to) << std::endl;

    return 0;
}