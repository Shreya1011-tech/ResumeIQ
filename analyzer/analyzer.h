#ifndef ANALYZER_H
#define ANALYZER_H

#include "../models/candidate.h"
#include <vector>
using namespace std;

struct AnalysisResult {
    vector<string> matched;
    vector<string> missing;
    int score;
};

AnalysisResult analyze(Candidate &c, string role);


vector<string> generateSuggestions(Candidate &c, AnalysisResult &res);

#endif