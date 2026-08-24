#include <print>
#include <vector>
#include <stack>

using namespace std;

void dfs(int currNode, const vector<vector<int>>& adjList, vector<bool>& visited, int traversal, stack<int>& order) {
    if (traversal == 1) order.push(currNode);
    for (int neigh: adjList[currNode]) {
        if (!visited[neigh]) {
            visited[neigh] = true;
            dfs(neigh, adjList, visited, traversal, order);
        }
    }
    if (traversal == 0) order.push(currNode);
}

vector<stack<int>> kosaraju(const vector<vector<int>>& adjList) {
    vector<bool> visited(adjList.size(), false);
    stack<int> order;

    for (int i = 0; i < adjList.size(); ++i) {
        if (!visited[i]) {
            visited[i] = true;
            dfs(i, adjList, visited, 0, order);
        }
    }

    vector<vector<int>> revAdjList(adjList.size());
    for (int i = 0; i < adjList.size(); ++i) {
        for (int neigh: adjList[i]) {
            revAdjList[neigh].push_back(i);
        }
    }

    fill(visited.begin(), visited.end(), false);
    vector<stack<int>> sccs;
    while (!order.empty()) {
        int currNode = order.top(); order.pop();
        if (!visited[currNode]) {
            visited[currNode] = true;
            sccs.push_back(stack<int>());
            dfs(currNode, revAdjList, visited, 1, sccs.back());
        }
    }

    return sccs;
}



int main() {
    // Graph:  0→1, 1→2, 2→0, 1→3, 3→4, 4→3
    // Expected SCCs: {0,1,2}  {3,4}
    vector<vector<int>> adj(5);
    adj[0].push_back(1);
    adj[1].push_back(2);
    adj[2].push_back(0);
    adj[1].push_back(3);
    adj[3].push_back(4);
    adj[4].push_back(3);

    vector<stack<int>> sccs = kosaraju(adj);

    for (auto& scc : sccs) {
        print("{{ ");
        while (!scc.empty()) {
            print(" {} ", scc.top());
            scc.pop();
        }
        print("}}\n");
    }

    return 0;
}
