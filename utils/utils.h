#ifndef UTILS_H
#define UTILS_H

#include "../ds/trie.h"
#include "../ds/graph.h"
#include "../models/candidate.h"
#include "../analyzer/analyzer.h"   // For AnalysisResult
#include <vector>
#include <string>
using namespace std;

// Utility functions
void loadSkills(Trie &trie);
void loadGraph(Graph &g);
void saveReport(Candidate &c, AnalysisResult &res, vector<string> &suggestions);
void saveHistory(Candidate &c, int score);
std::string trim(string str);
string toLower(string str);
#endif