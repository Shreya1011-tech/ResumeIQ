#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include "../analyzer/analyzer.h"
#include "utils.h"

using namespace std;

// -----------------------------
// Save Resume Analysis Report
// -----------------------------
void saveReport(Candidate &c, AnalysisResult &res, vector<string> &suggestions) {
    ofstream file("output/reports/report.txt");

    if (!file) {
        cout << "Error saving report!\n";
        return;
    }

    file << "===== RESUME ANALYSIS REPORT =====\n\n";
    file << "Name: " << c.name << endl;
    file << "Email: " << c.email << endl;

    file << "\nSkills: ";
    for (auto s : c.skills) file << s << ", ";

    file << "\n\nScore: " << res.score << "%\n";

    file << "\nMatched Skills: ";
    for (auto s : res.matched) file << s << ", ";

    file << "\nMissing Skills: ";
    for (auto s : res.missing) file << s << ", ";

    file << "\n\nSuggestions:\n";
    int i = 1;
    for (auto s : suggestions) file << i++ << ". " << s << endl;

    file.close();
}

// -----------------------------
// Trim function (remove spaces)
// -----------------------------
string trim(string str) {
    int start = 0, end = str.length() - 1;
    while (start <= end && isspace(str[start])) start++;
    while (end >= start && isspace(str[end])) end--;
    return str.substr(start, end - start + 1);
}

// -----------------------------
// Load skill graph from file
// -----------------------------
void loadGraph(Graph &g) {
    ifstream file("config/skill_graph.txt");
    if (!file) {
        cout << "Error loading skill_graph.txt\n";
        return;
    }

    string u, v;
    while (file >> u >> v) {
        transform(u.begin(), u.end(), u.begin(), ::tolower);
        transform(v.begin(), v.end(), v.begin(), ::tolower);
        g.addEdge(u, v);
    }

    file.close();
}

// -----------------------------
// Load skills into Trie
// -----------------------------
void loadSkills(Trie &trie) {
    ifstream file("config/roles.txt");
    if (!file) {
        cout << "Error loading roles.txt\n";
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.find(":") != string::npos) continue; // skip role names
        if (!line.empty()) {
            transform(line.begin(), line.end(), line.begin(), ::tolower);
            trie.insert(line);
        }
    }

    file.close();
}

// -----------------------------
// Save candidate history
// -----------------------------
void saveHistory(Candidate &c, int score) {
    ofstream file("output/history.txt", ios::app);
    if (!file) {
        cout << "Error saving history!\n";
        return;
    }
    file << c.name << " | " << c.email << " | Score: " << score << "%\n";
    file.close();
}
string toLower(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}