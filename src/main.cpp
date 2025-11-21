#include <iostream>
#include <limits>

// Include your headers
#include "structures.h" // Good to have, though globals.h brings it in
#include "globals.h"
#include "utils.h"
#include "features.h"
#include "dijkstra.h"
#include "graph.h"

// --- UNITY BUILD: Include CPP files directly ---
// Order matters!
#include "globals.cpp"
#include "utils.cpp"
#include "graph.cpp"
#include "dijkstra.cpp"
#include "features.cpp"

using namespace std;

int main() {
    showLoadingScreen();

    if (allNodes.empty()) {
        cout << "Error: Graph is empty." << endl;
        return 1;
    }

    while (true) {
        printHeader("Main Menu");
        cout << "   [1] Calculate Shortest Route" << endl;
        cout << "   [2] Browse Landmarks" << endl;
        cout << "   [3] Emergency Services (Police/Hospital/Fire)" << endl;
        cout << "   [4] Public Transport (Find Nearest Bus)" << endl;
        cout << "   [5] Fuel Station (Find Petrol Pump)" << endl;
        cout << "   [6] Exit System" << endl;
        cout << endl;
        cout << "   Select Option >> ";
        
        int choice;
        if (!(cin >> choice)) { 
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(); 

        if (choice == 6) {
            cout << "\n   Exiting... Goodbye!" << endl;
            break;
        }
        else if (choice == 2) {
            showLandmarks();
        }
        else if (choice == 1) {
            string sourceInput, destInput, sourceID, destID;
            
            while(true) {
                printHeader("Plan Route: Step 1/3");
                cout << "   Enter Start Location (Name): ";
                getline(cin, sourceInput);
                string key = normalizeString(sourceInput);
                if (searchLookup.find(key) != searchLookup.end()) {
                    sourceID = searchLookup[key];
                    sourceInput = idToLandmark[sourceID]; 
                    break;
                }
                cout << "\n   [!] Error: Location not found. Press Enter to retry.";
                cin.get();
            }

            while(true) {
                printHeader("Plan Route: Step 2/3");
                cout << "   From: " << sourceInput << endl;
                cout << "   -----------------------------------" << endl;
                cout << "   Enter Destination (Name): ";
                getline(cin, destInput);
                string key = normalizeString(destInput);
                if (searchLookup.find(key) != searchLookup.end()) {
                    destID = searchLookup[key];
                    destInput = idToLandmark[destID]; 
                    break;
                }
                cout << "\n   [!] Error: Location not found. Press Enter to retry.";
                cin.get();
            }

            printHeader("Plan Route: Step 3/3");
            cout << "   From: " << sourceInput << endl;
            cout << "   To:   " << destInput << endl;
            cout << "   -----------------------------------" << endl;
            cout << "   [D] Detailed View (All Intersections)" << endl;
            cout << "   [L] Landmarks Only (Clean View)" << endl;
            cout << endl;
            cout << "   Select Preference >> ";
            char viewType;
            cin >> viewType;
            bool showDetailed = (viewType == 'd' || viewType == 'D');
            cin.ignore(); 

            printHeader("Calculating...");
            Node* sourceNode = nodeLookup[sourceID];
            Node* destNode = nodeLookup[destID];
            
            if(sourceNode->adj.empty()) {
                cout << "\n   [!] CRITICAL DATA ERROR: Start location is isolated." << endl;
                cin.get();
                continue;
            }
            
            Dijkstras(sourceNode);
            PrintShortestRouteTo(destNode, sourceNode, destInput, sourceInput, showDetailed);
        }
        else if (choice == 3) {
            while(true) {
                printHeader("Emergency Services");
                cout << "   Find the nearest service to your location:" << endl;
                cout << "   ----------------------------------------" << endl;
                cout << "   [1] Police Station" << endl;
                cout << "   [2] Hospital / Medical Centre" << endl;
                cout << "   [3] Fire Station" << endl;
                cout << "   [4] Back to Main Menu" << endl;
                cout << endl;
                cout << "   Select Option >> ";

                int subChoice;
                if (!(cin >> subChoice)) { 
                    cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                cin.ignore();

                if (subChoice == 4) break;
                
                string keyword = "";
                if (subChoice == 1) keyword = "police";
                else if (subChoice == 2) keyword = "hospital"; 
                else if (subChoice == 3) keyword = "fire";
                else continue;

                string userLoc;
                cout << "\n   Enter your current location: ";
                getline(cin, userLoc);

                FindClosestService(userLoc, keyword, 0); 
            }
        }
        else if (choice == 4) {
            printHeader("Public Transport (Bus)");
            cout << "   Find the nearest Bus Station." << endl;
            cout << "   -----------------------------" << endl;
            string userLoc;
            cout << "   Enter your current location: ";
            getline(cin, userLoc);
            FindClosestService(userLoc, "bus", 1); 
        }
        else if (choice == 5) {
            printHeader("Fuel Station Locator");
            cout << "   Find the nearest Petrol Pump/Station." << endl;
            cout << "   -------------------------------------" << endl;
            string userLoc;
            cout << "   Enter your current location: ";
            getline(cin, userLoc);
            FindClosestService(userLoc, "petrol", 2); 
        }
    }

    return 0;
}