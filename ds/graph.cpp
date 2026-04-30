#include "graph.h"

void Graph::addEdge(const string &u, const string &v) {
    adj[u].push_back(v);
}

void Graph::printPath(const vector<string> &knownSkills, const vector<string> &missingSkills) {
    set<string> visited;
    queue<pair<string, vector<string>>> q;

    for (auto &k : knownSkills) {
        q.push({k, {k}});
        visited.insert(k);
    }

    bool found = false;

    while (!q.empty()) {
        auto curr = q.front(); q.pop();
        string skill = curr.first;
        vector<string> path = curr.second;

        // print ANY path longer than 1 (learning path)
        if (path.size() > 1) {
            for (size_t i = 0; i < path.size(); i++) {
                cout << path[i];
                if (i != path.size() - 1) cout << " -> ";
            }
            cout << endl;
            found = true;
        }

        for (auto &neighbor : adj[skill]) {
            if (!visited.count(neighbor)) {
                visited.insert(neighbor);
                vector<string> newPath = path;
                newPath.push_back(neighbor);
                q.push({neighbor, newPath});
            }
        }
    }

    if (!found) {
        cout << "No learning path found\n";
    }
}