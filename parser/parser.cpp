#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include "parser.h"
#include "../utils/utils.h"

using namespace std;

// check keyword match
bool containsKeyword(string line, vector<string> keywords) {
    for (auto k : keywords) {
        if (line.find(k) != string::npos)
            return true;
    }
    return false;
}

// simple email detection
bool isEmail(string line) {
    return line.find("@") != string::npos;
}

// check if number
bool isNumber(string s) {
    for (char c : s) {
        if (!isdigit(c)) return false;
    }
    return !s.empty();
}

Candidate parseResume(string filename) {
    Candidate c;
    ifstream file(filename);

    if (!file) {
        cout << "Error opening file!\n";
        return c;
    }

    string line;
    unordered_set<string> uniqueSkills;

    vector<string> skillKeywords = {"skills", "technical", "technologies", "stack"};
    vector<string> expKeywords = {"experience", "years"};
    vector<string> projKeywords = {"projects"};
    vector<string> certKeywords = {"certifications", "certificates"};

    while (getline(file, line)) {
        string lower = toLower(line);

        // name (first line assumption)
        if (c.name.empty() && !line.empty()) {
            c.name = line;
        }

        // email detection
        if (isEmail(line)) {
    size_t pos = line.find("@");

    // find start of email
    size_t start = line.rfind(" ", pos);
    if (start == string::npos) start = 0;
    else start++;

    // find end of email
    size_t end = line.find(" ", pos);
    if (end == string::npos) end = line.length();

    c.email = line.substr(start, end - start);
}

        // detect experience (number in line)
        if (containsKeyword(lower, expKeywords)) {
            stringstream ss(line);
            string word;
            while (ss >> word) {
                if (isNumber(word)) {
                    c.experience = stoi(word);
                }
            }
        }

        // detect projects
        if (containsKeyword(lower, projKeywords)) {
            stringstream ss(line);
            string word;
            while (ss >> word) {
                if (isNumber(word)) {
                    c.projects = stoi(word);
                }
            }
        }

        // detect certifications
        if (containsKeyword(lower, certKeywords)) {
            stringstream ss(line);
            string word;
            while (ss >> word) {
                if (isNumber(word)) {
                    c.certifications = stoi(word);
                }
            }
        }

        // 🔥 SKILL EXTRACTION (MOST IMPORTANT)
        stringstream ss(line);
        string word;
        while (ss >> word) {
            word = toLower(word);

            // remove commas
            if (!word.empty() && word.back() == ',')
                word.pop_back();

            // simple filter
            if (word.length() > 2 && isalpha(word[0])) {
                uniqueSkills.insert(word);
            }
        }
    }

    // move set → vector
    for (auto s : uniqueSkills) {
        c.skills.push_back(s);
    }

    file.close();
    return c;
}