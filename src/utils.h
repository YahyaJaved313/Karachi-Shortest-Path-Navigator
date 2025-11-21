#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream>
#include <cstdlib>
#include <cctype>

using namespace std;

void clearScreen();
void printHeader(string title);
void printDivider();
string normalizeString(string input);
bool stringContains(string mainStr, string subStr);
bool hasWholeWord(string text, string word);

#endif