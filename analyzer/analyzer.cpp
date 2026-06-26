#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include "analyzer.h"
#include "scoring.h"
#include "../utils/utils.h"
#include "../libs/json.hpp"  

using namespace std;
using json = nlohmann::json;  //used for reading json file
// -----------------------------
// Helper: lowercase a string
// -----------------------------
// -----------------------------
// Load skills for a given role
// -----------------------------
vector<string> getRoleSkills(string role) {

    // Step 1: Normalize input
    role = toLower(role);

    // Step 2: Open JSON file
    ifstream file("config/roles.json");

    // Step 3: Check file opened
    if (!file.is_open()) {
        cout << "Error: config/roles.json not found!\n";
        return {};
    }

    // Step 4: Parse JSON
    json data;
    try {
        data = json::parse(file);
    }
    catch (json::parse_error& e) {
        cout << "Error reading roles.json: " << e.what() << "\n";
        return {};
    }

    // Step 5: Check role exists
    if (data.find(role) == data.end()) {
        cout << "Role not found: " << role << "\n";
        cout << "Available roles:\n";
        for (auto& item : data.items()) {
            cout << "  - " << item.key() << "\n";
        }
        return {};
    }

    // Step 6: Extract and return skills
    vector<string> skills;
    for (string skill : data[role]) {
        skills.push_back(skill);
    }
cout << "DEBUG: Found " << skills.size() << " skills for role: " << role << "\n";

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