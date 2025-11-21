#ifndef FEATURES_H
#define FEATURES_H

#include <string>
// --- FIX: Include the structures file ---
#include "structures.h"

using namespace std;

void PrintShortestRouteTo(Node* destination, Node* source, string destName, string sourceName, bool showDetailed);
void FindClosestService(string userLocationName, string serviceKeyword, int searchMode);
void showLandmarks();
void showLoadingScreen();

#endif