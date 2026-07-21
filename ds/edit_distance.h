#ifndef EDIT_DISTANCE_H
#define EDIT_DISTANCE_H

#include <string>
#include <vector>
#include <algorithm>
#include<climits>
using namespace std;

// -----------------------------
// Calculate Edit Distance
// between two strings
// -----------------------------
int editDistance(string word1, string word2) {
    int m = word1.length();
    int n = word2.length();

    // Create 2D table filled with 0s
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

    // Fill first column — cost of deleting all chars of word1
    for (int i = 0; i <= m; i++)
        dp[i][0] = i;

    // Fill first row — cost of inserting all chars of word2
    for (int j = 0; j <= n; j++)
        dp[0][j] = j;

    // Fill rest of table
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {

            if (word1[i-1] == word2[j-1]) {
                // Characters match — no cost
                dp[i][j] = dp[i-1][j-1];
            }
            else {
                // Characters don't match — take minimum operation
                dp[i][j] = 1 + min({
                    dp[i-1][j],      // delete
                    dp[i][j-1],      // insert
                    dp[i-1][j-1]     // replace
                });
            }
        }
    }

    // Answer is bottom-right cell
    return dp[m][n];
}

// -----------------------------
// Find closest matching skill
// from known skills list
// -----------------------------
string findClosestSkill(string input, vector<string> knownSkills) {
    string closest = "";
    int minDist = INT_MAX;

    for (string skill : knownSkills) {
        int dist = editDistance(input, skill);
        if (dist < minDist) {
            minDist = dist;
            closest = skill;
        }
    }

    // Only return if close enough (threshold = 2)
    if (minDist <= 2)
        return closest;
    else
        return "";  // too different — not a valid skill
}

#endif