#include <iostream>
#include<fstream>
#include <vector>
#include <string>
#include <sys/stat.h>   // for _mkdir
#include <direct.h>     // Windows _mkdir
#include "menu/menu.h"
#include "parser/parser.h"
#include "ds/trie.h"
#include "utils/utils.h"
#include "analyzer/analyzer.h"
#include "ds/graph.h"

using namespace std;

// -----------------------------
// Folder creation function
// Ensures output folders exist
// -----------------------------
void createFolder(const char* folderName) {
    struct stat info;
    if (stat(folderName, &info) != 0) {  // folder doesn't exist
        _mkdir(folderName);               // create folder
    }
}

int main() {
    int choice;

    // Ensure folders exist before anything else
    createFolder("output");
    createFolder("output/reports");

    while (true) {
        showMenu();
        cin >> choice;

        // -----------------------------
        // Option 1: Analyze Resume
        // -----------------------------
        if (choice == 1) {
            string filename, role;
            cout << "Enter resume file name: ";
            cin >> filename;

            cout << "Enter role to analyze against: ";
            cin.ignore(); // ignore leftover newline
            getline(cin, role);

            // Parse resume into Candidate object
            Candidate c = parseResume(filename);

            // -----------------------------
            // Skill Matching using Trie
            // -----------------------------
            Trie trie;
            loadSkills(trie);  // loads all role skills into trie
            vector<string> filteredSkills;

            for (auto s : c.skills)
                if (trie.search(s)) filteredSkills.push_back(s);

            c.skills = filteredSkills;

            // -----------------------------
            // Analyze and generate suggestions
            // -----------------------------
            AnalysisResult res = analyze(c, role);
            vector<string> suggestions = generateSuggestions(c, res);

            // -----------------------------
            // Terminal Output (Pretty)
            // -----------------------------
            cout << "\n===== PARSED RESUME =====\n";
            cout << "Name: " << c.name << "\n";
            cout << "Email: " << c.email << "\n";
            cout << "Skills: ";
            for (auto s : c.skills) cout << s << ", ";
            cout << "\nExperience: " << c.experience;
            cout << "\nProjects: " << c.projects;
            cout << "\nCertifications: " << c.certifications << "\n";

            cout << "\n===== ANALYSIS =====\n";
            cout << "Score: " << res.score << "%\n";
            cout << "Matched Skills: ";
            for (auto s : res.matched) cout << s << ", ";
            cout << "\nMissing Skills: ";
            for (auto s : res.missing) cout << s << ", ";

            // -----------------------------
            // Learning Path from Skill Graph
            // -----------------------------
            cout << "\n\n===== LEARNING PATH =====\n";
            Graph g;
            loadGraph(g);
            g.printPath(c.skills, res.missing); // prints: skill1 -> skill2 -> ...

            // -----------------------------
            // Suggestions
            // -----------------------------
            cout << "\n===== SUGGESTIONS =====\n";
            int i = 1;
            for (auto s : suggestions) cout << i++ << ". " << s << "\n";

            // -----------------------------
            // Save report & history
            // -----------------------------
            saveReport(c, res, suggestions);
            saveHistory(c, res.score);
            cout << "\nReport saved successfully!\n";
        }

        // -----------------------------
        // Option 2: View Analysis History
        // -----------------------------
        else if (choice == 2) {
    std::ifstream file("output/history.txt");
    if (!file) {
        std::cout << "No history found.\n";
    } else {
        std::cout << "\n===== ANALYSIS HISTORY =====\n";
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << std::endl;
        }
        std::cout << "=============================\n";
        file.close();
    }
}

        // -----------------------------
        // Option 3: Exit
        // -----------------------------
        else if (choice == 3) {
            cout << "Exiting...\n";
            return 0;
        }
        
        else if (choice == 4) {
    ofstream file("output/history.txt", ios::trunc);

    if (!file) {
        cout << "Error clearing history!\n";
    } else {
        cout << "History cleared successfully!\n";
    }

    file.close();
}
        else {
            cout << "Invalid choice! Try again.\n";
        }
    }
}