#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <deque>

using namespace std;

void print(unordered_map<string, vector<string>> map) {
    for (const auto& [p, hateList]: map) {
        cout << p << ": ";
        for (const string& hateP: hateList) {
            cout << hateP << ", ";
        }
        cout << endl;
    }
}

bool canBeDividedInTwoGroups(const vector<string>& roster, const vector<tuple<string, string>>& hateList) {
    unordered_map<string, vector<string>> adjList;
    unordered_map<string, int> groupAssignment;
    for (const string& p: roster) {
        adjList[p] = vector<string>();
        groupAssignment[p] = -1;
    }
    for (const auto& [p1, p2]: hateList) {
        adjList[p1].push_back(p2);
        adjList[p2].push_back(p1);
    }
    print(adjList);

    // -1 unassigned, 0 is group0 and 1 is group1
    for (const auto& [p, group]: groupAssignment) {
        if (group == -1) {
            deque<string> next;
            next.push_back(p);
            groupAssignment[p] = 0;

            while (next.size() > 0) {
                string p = next.front(); next.pop_front();
                for (string hatedP: adjList[p]) {
                    if (groupAssignment[hatedP] == -1) {
                        groupAssignment[hatedP] = 1 - groupAssignment[p];
                        next.push_back(hatedP);
                    }
                    else if (groupAssignment[p] == groupAssignment[hatedP]) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

int main() {
    vector<string> roster({"A", "B", "C", "D", "E", "F", "G", "H"});
    vector<tuple<string, string>> hateList({{"A", "B"}, {"B", "C"}, {"C", "A"}, {"D", "E"}});
    std::cout << canBeDividedInTwoGroups(roster, hateList) << std::endl;
    return 0;
}
