#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <set>
#include <optional>

using namespace std;

void print(unordered_map<string, int> map) {
    for (const auto& [p, grp]: map) {
        cout << p << ": " << grp << endl;
    }
}

std::optional<unordered_map<string, int>> allocateToGroups(const vector<string>& roster, const vector<tuple<string, string>> hateList, int numGroups) {
    unordered_map<string, vector<string>> adjList;
    unordered_map<string, int> groupAssignments;
    for (const string& p: roster) {
        adjList[p] = vector<string>();
        groupAssignments[p] = -1;
    }
    for (const auto& [p1, p2]: hateList) {
        adjList[p1].push_back(p2);
        adjList[p2].push_back(p1);
    }

    for (const auto& [p, hatedPeople]: adjList) {
        set<int> bannedGroups;
        for (const string& hp: hatedPeople) {
            if (groupAssignments[hp] != -1) bannedGroups.insert(groupAssignments[hp]);
        }
        for (int i = 0; i < numGroups; ++i) {
            if (bannedGroups.count(i) == 0) {
                groupAssignments[p] = i;
                break;
            }
            else if (i == numGroups-1) return nullopt;
        }
    }
    return groupAssignments;
}

int main() {
    vector<string> roster({"A", "B", "C", "D", "E", "F", "G", "H"});
    vector<tuple<string, string>> hateList({{"A", "B"}, {"B", "C"}, {"C", "A"}, {"D", "E"}});
    auto groupAssignments = allocateToGroups(roster, hateList, 3);
    if (groupAssignments) print(groupAssignments.value());
    return 0;
}
