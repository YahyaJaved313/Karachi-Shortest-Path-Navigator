#include "features.h"
#include "globals.h"
#include "utils.h"
#include "dijkstra.h"
#include "graph.h"
#include <iostream>
#include <algorithm>
#include <limits>

using namespace std;

void PrintShortestRouteTo(Node* destination, Node* source, string destName, string sourceName, bool showDetailed) {
    printHeader("Route Calculation Results");

    if (destination->distanceFromStart == MAX_DISTANCE) {
        cout << "\n   [!] ERROR: No path found." << endl;
        cout << "   --------------------------------------------------" << endl;
        cout << "   DIAGNOSIS:" << endl;
        
        if (source->adj.empty()) {
             cout << "   The Start Location '" << sourceName << "' is ISOLATED." << endl;
             cout << "   (It exists in the map, but no roads connect to it in the data file.)" << endl;
        }
        else if (destination->adj.empty()) {
             cout << "   The Destination '" << destName << "' is ISOLATED." << endl;
             cout << "   (It exists in the map, but no roads connect to it in the data file.)" << endl;
        } 
        else {
             cout << "   Both locations have roads, but they belong to separate" << endl;
             cout << "   disconnected networks (Islands) with no bridge between them." << endl;
        }
        
        cout << "\n   Press Enter to return...";
        cin.ignore(); cin.get();
        return;
    }

    vector<string> rawPath;
    Node* curr = destination;
    while (curr != NULL) {
        rawPath.push_back(curr->id);
        curr = curr->previous;
    }
    reverse(rawPath.begin(), rawPath.end());

    vector<string> printList;
    for (size_t i = 0; i < rawPath.size(); ++i) {
        string id = rawPath[i];
        bool isLandmark = (idToLandmark.find(id) != idToLandmark.end());

        if (showDetailed) {
            if (isLandmark) printList.push_back(idToLandmark[id]);
            else printList.push_back(id);
        } else {
            if (isLandmark) printList.push_back(idToLandmark[id]);
        }
    }

    cout << "   TRIP SUMMARY" << endl;
    cout << "   ------------" << endl;
    cout << "   From:       " << sourceName << endl;
    cout << "   To:         " << destName << endl;
    cout << "   Distance:   " << destination->distanceFromStart << " meters" << endl;
    cout << "   Stops:      " << printList.size() << " items shown" << endl; 
    printDivider();

    cout << "   TURN-BY-TURN DIRECTIONS" << endl;
    cout << "   -----------------------" << endl;

    if (printList.empty()) {
        cout << "   (Path is strictly passing through unnamed roads)" << endl;
    } else {
        for (size_t i = 0; i < printList.size(); ++i) {
            if (i == 0) cout << "   (START)  "; 
            else if (i == printList.size() - 1) cout << "   ( END )  ";
            else cout << "      |     ";

            cout << printList[i] << endl;

            if (i != printList.size() - 1) {
                cout << "      |     " << endl;
                cout << "      v     " << endl;
            }
        }
    }
    
    printDivider();
    cout << "   Press Enter to return to menu...";
    cin.ignore(); cin.get();
}

void FindClosestService(string userLocationName, string serviceKeyword, int searchMode) {
    string key = normalizeString(userLocationName);
    if (searchLookup.find(key) == searchLookup.end()) {
        cout << "\n   [!] Error: Your location '" << userLocationName << "' was not found." << endl;
        cout << "   Press Enter to return...";
        cin.get();
        return;
    }

    string startNodeID = searchLookup[key];
    string properStartName = idToLandmark[startNodeID];
    Node* startNode = nodeLookup[startNodeID];

    if (startNode->adj.empty()) {
        cout << "\n   [!] DATA ERROR: Your location '" << properStartName << "' is isolated." << endl;
        cout << "   It has no connecting roads in the dataset, so we cannot find a route." << endl;
        cout << "   Press Enter to return...";
        cin.get();
        return;
    }

    printHeader("Locating Nearest Service...");
    if (searchMode == 2) 
        cout << "   Searching for nearest Fuel Station from " << properStartName << "..." << endl;
    else 
        cout << "   Searching for nearest '" << serviceKeyword << "' from " << properStartName << "..." << endl;

    Dijkstras(startNode);

    Node* closestNode = NULL;
    string closestName = "";
    int minDistance = MAX_DISTANCE;

    for (auto const& [name, id] : landmarkMap) {
        string normName = normalizeString(name);
        bool isMatch = false;

        if (searchMode == 0) {
            if (stringContains(normName, serviceKeyword)) isMatch = true;
        } 
        else if (searchMode == 1) {
            if (hasWholeWord(normName, serviceKeyword)) isMatch = true;
        }
        else if (searchMode == 2) {
            if (stringContains(normName, "petrol_pump") || stringContains(normName, "petrol_station")) {
                isMatch = true;
            }
        }

        if (isMatch) {
            if (nodeLookup.find(id) != nodeLookup.end()) {
                Node* target = nodeLookup[id];
                if (target->distanceFromStart != MAX_DISTANCE && target->distanceFromStart < minDistance) {
                    minDistance = target->distanceFromStart;
                    closestNode = target;
                    closestName = name;
                }
            }
        }
    }

    if (closestNode != NULL) {
        cout << "   Found: " << closestName << endl;
        cout << "   Distance: " << minDistance << " meters" << endl;
        cout << "\n   Press Enter to view route details...";
        cin.get();
        
        PrintShortestRouteTo(closestNode, startNode, closestName, properStartName, false); 
    } else {
        if (searchMode == 2)
            cout << "\n   [!] No Fuel Stations reachable from your location." << endl;
        else
            cout << "\n   [!] No '" << serviceKeyword << "' reachable from your location." << endl;
            
        cout << "   (They might be too far or disconnected from the road network)" << endl;
        cout << "   Press Enter to return...";
        cin.get();
    }
}

void showLandmarks() {
    printHeader("Available Landmarks Repository");
    
    int count = 0;
    int pageSize = 20; 

    auto it = landmarkMap.begin();
    while (it != landmarkMap.end()) {
        for (int i = 0; i < pageSize && it != landmarkMap.end(); ++i) {
            cout << "   - " << it->first << endl;
            ++it;
            count++;
        }

        if (it == landmarkMap.end()) {
            cout << "\n   [End of List]" << endl;
            break;
        }

        cout << "\n   -- Showing " << count << "/" << landmarkMap.size() << " --" << endl;
        cout << "   [Enter] Next Page | [Q] Quit to Menu: ";
        
        string input;
        getline(cin, input);
        if (!input.empty() && (input[0] == 'q' || input[0] == 'Q')) break;
        
        printHeader("Available Landmarks (Cont.)");
    }
}

void showLoadingScreen() {
    clearScreen();
    cout << "\n\n\n";
    cout << "          SYSTEM INITIALIZING          " << endl;
    cout << "   ==================================" << endl;
    cout << "   [+] Reading Map Data... ";
    createGraph();
    cout << "   [+] Indexing Landmarks... ";
    loadLandmarks();
    cout << "   ==================================" << endl;
    cout << "          READY. Press Enter." << endl;
    cin.get();
}