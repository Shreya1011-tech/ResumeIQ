#ifndef CANDIDATE_H
#define CANDIDATE_H

#include <iostream>
#include <vector>
using namespace std;

class Candidate {
public:
    string name;
    string email;
    vector<string> skills;
    int experience = 0;
    int projects = 0;
    bool internship = false;
    int certifications = 0;
};

#endif