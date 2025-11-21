#include "globals.h"

// Define the variables (allocate memory for them)
vector<Node*> allNodes;
unordered_map<string, Node*> nodeLookup; 
map<string, string> landmarkMap;            
unordered_map<string, string> idToLandmark; 
unordered_map<string, string> searchLookup;