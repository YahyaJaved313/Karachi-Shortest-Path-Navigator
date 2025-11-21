#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include <unordered_map>
#include <map>
#include <string>

// --- FIX: Include the structures file ---
#include "structures.h" 

using namespace std;

extern vector<Node*> allNodes;
extern unordered_map<string, Node*> nodeLookup; 
extern map<string, string> landmarkMap;            
extern unordered_map<string, string> idToLandmark; 
extern unordered_map<string, string> searchLookup; 

#endif