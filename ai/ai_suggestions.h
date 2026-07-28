#ifndef AI_SUGGESTIONS_H
#define AI_SUGGESTIONS_H

#include <string>
#include <vector>
using namespace std;

string getAISuggestions(string role,
                         vector<string> matched,
                         vector<string> missing,
                         int experience);
#endif