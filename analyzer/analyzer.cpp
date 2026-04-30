#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include "analyzer.h"
#include "scoring.h"
#include "../utils/utils.h"

using namespace std;

// -----------------------------
// Helper: lowercase a string
// -----------------------------
// -----------------------------
// Load skills for a given role
// -----------------------------
vector<string> getRoleSkills(string role) {
    ifstream file("config/roles.txt");
    string line;
    vector<string> skills;
    bool found = false;

    role = toLower(role); // lowercase role for comparison

    while (getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        string lowerLine = toLower(line);

        // Check for role header
        if (lowerLine == role + ":") {
            found = true;
            continue;
        }

        // After role header, collect skills until empty line or new role
        if (found) {
            if (line.empty() || line.find(":") != string::npos)
                break;
            skills.push_back(toLower(line));
        }
    }

    file.close();
    return skills;
}

// -----------------------------
// Analyze candidate against role
// -----------------------------
AnalysisResult analyze(Candidate &c, string role) {
    AnalysisResult result;

    vector<string> roleSkills = getRoleSkills(role);

    // Normalize candidate skills
    unordered_set<string> candidateSet;
    for (auto s : c.skills)
        candidateSet.insert(toLower(s));

    // Compare skills
    for (auto skill : roleSkills) {
        if (candidateSet.count(skill)) {
            result.matched.push_back(skill);
        } else {
            result.missing.push_back(skill);
        }
    }

    // Compute total score
    if (!roleSkills.empty())
        result.score = calculateScore(c, result);
    else
        result.score = 0;

    return result;
}

// -----------------------------
// Generate suggestions
// -----------------------------
vector<string> generateSuggestions(Candidate &c, AnalysisResult &res) {
    vector<string> suggestions;

    // Missing skills
    for (auto skill : res.missing)
        suggestions.push_back("Learn " + skill);

    // Projects
    if (c.projects < 2)
        suggestions.push_back("Build more projects");

    // Experience
    if (c.experience < 1)
        suggestions.push_back("Gain practical experience (internship)");

    // Certifications
    if (c.certifications == 0)
        suggestions.push_back("Complete at least 1 certification");

    return suggestions;
}