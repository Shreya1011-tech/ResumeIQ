#ifndef AI_SUGGESTIONS_H
#define AI_SUGGESTIONS_H

#include <string>
#include <vector>
using namespace std;

// Declared here — defined in ai_suggestions.cpp
string getAISuggestions(string role,
                         vector<string> matched,
                         vector<string> missing);

#endif