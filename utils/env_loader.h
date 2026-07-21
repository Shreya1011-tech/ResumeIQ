#ifndef ENV_LOADER_H
#define ENV_LOADER_H

#include <fstream>
#include <string>
#include <iostream>
using namespace std;

string getEnvValue(string keyName) {
    ifstream file(".env");

    if (!file.is_open()) {
        cout << "Warning: .env file not found!\n";
        return "";
    }

    string line;
    while (getline(file, line)) {
        int equalPos = line.find('=');
        if (equalPos == string::npos)
            continue;

        string key   = line.substr(0, equalPos);
        string value = line.substr(equalPos + 1);

        if (key == keyName) {
            return value;
        }
    }

    cout << "Warning: " << keyName << " not found in .env\n";
    return "";
}

#endif