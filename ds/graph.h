#ifndef GRAPH_H
#define GRAPH_H
#include <unordered_map>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include <iostream>
#include <algorithm>
using namespace std;

class Graph {
    unordered_map<string, vector<string>> adj;
public:
    void addEdge(const string &u, const string &v);
    void printPath(const vector<string> &knownSkills, const vector<string> &missingSkills);
};

#endif