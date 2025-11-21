#include "utils.h"

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void printHeader(string title) {
    clearScreen();
    cout << "============================================================" << endl;
    cout << "   KARACHI NAVIGATION SYSTEM " << endl;
    cout << "============================================================" << endl;
    cout << " > " << title << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << endl;
}

void printDivider() {
    cout << "\n------------------------------------------------------------\n";
}

string normalizeString(string input) {
    string result = "";
    for (char c : input) {
        if (c == ' ') result += '_';
        else result += tolower(c);
    }
    return result;
}

bool stringContains(string mainStr, string subStr) {
    return mainStr.find(subStr) != string::npos;
}

bool hasWholeWord(string text, string word) {
    string padded = "_" + text + "_";
    string searchPattern = "_" + word + "_";
    return padded.find(searchPattern) != string::npos;
}