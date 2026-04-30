#include "scoring.h"

int calculateScore(Candidate &c, AnalysisResult &res) {
    int score = 0;

    // 🔹 Skills (50)
    int totalSkills = res.matched.size() + res.missing.size();
    if (totalSkills > 0) {
        score += (res.matched.size() * 50) / totalSkills;
    }

    // 🔹 Experience (15)
    if (c.experience >= 2)
        score += 15;
    else if (c.experience == 1)
        score += 8;

    // 🔹 Projects (15)
    if (c.projects >= 3)
        score += 15;
    else if (c.projects == 2)
        score += 10;
    else if (c.projects == 1)
        score += 5;

    // 🔹 Certifications (10)
    if (c.certifications >= 2)
        score += 10;
    else if (c.certifications == 1)
        score += 5;

    // 🔹 Internship (10)
    if (c.internship)
        score += 10;

    return score;
}