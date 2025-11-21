#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <map>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <cctype>
#include <cstdlib> 
#include <iomanip> 

using namespace std;

// Use max int for infinity
const int MAX_DISTANCE = numeric_limits<int>::max();

// --- Forward Declarations ---
class Node;

// --- Edge Class ---
class Edge {
public:
    Node* node1;
    Node* node2;
    int distance;

    Edge(Node* n1, Node* n2, int dist) : node1(n1), node2(n2), distance(dist) {}
};

// --- Node Class ---
class Node {
public:
    string id;
    int distanceFromStart;
    Node* previous;
    vector<Edge*> adj; 

    Node(string id) : id(id), distanceFromStart(MAX_DISTANCE), previous(NULL) {}
};

// --- Global Storage ---
vector<Node*> allNodes;
unordered_map<string, Node*> nodeLookup; 
map<string, string> landmarkMap;            
unordered_map<string, string> idToLandmark; 
unordered_map<string, string> searchLookup; 

// --- UI Helpers ---

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

// --- New Helper: Whole Word Match ---
// Checks if 'word' exists as a distinct word (surrounded by underscores or start/end)
bool hasWholeWord(string text, string word) {
    // Pad the text with underscores so we can always search for "_word_"
    // Example: "bus_stop" -> "_bus_stop_" contains "_bus_"
    // Example: "business" -> "_business_" does NOT contain "_bus_"
    string padded = "_" + text + "_";
    string searchPattern = "_" + word + "_";
    return padded.find(searchPattern) != string::npos;
}

// --- Comparator for Dijkstra ---
struct NodeCompare {
    bool operator()(Node* a, Node* b) {
        return a->distanceFromStart > b->distanceFromStart;
    }
};

// --- 1. Graph Creation ---
void createGraph() {
    ifstream locationsFile("karachi_locations.txt");
    if (!locationsFile.is_open()) {
        cerr << "Error: Could not open karachi_locations.txt." << endl;
        return;
    }

    string nodeID;
    while (locationsFile >> nodeID) {
        Node* newNode = new Node(nodeID);
        allNodes.push_back(newNode);
        nodeLookup[nodeID] = newNode; 
    }
    locationsFile.close();

    ifstream roadsFile("karachi_roads.txt");
    if (!roadsFile.is_open()) {
        cerr << "Error: Could not open karachi_roads.txt" << endl;
        return;
    }

    string u, v;
    int dist;
    while (roadsFile >> u >> v >> dist) {
        if (nodeLookup.find(u) != nodeLookup.end() && nodeLookup.find(v) != nodeLookup.end()) {
            Node* n1 = nodeLookup[u];
            Node* n2 = nodeLookup[v];
            Edge* newEdge = new Edge(n1, n2, dist);
            n1->adj.push_back(newEdge);
            n2->adj.push_back(newEdge);
        }
    }
    roadsFile.close();
}

// --- 2. Load Landmarks ---
void loadLandmarks() {
    ifstream file("karachi_landmarks.txt");
    if (!file.is_open()) {
        cerr << "Warning: Could not open karachi_landmarks.txt" << endl;
        return;
    }
    
    string name, id;
    while (file >> name >> id) {
        landmarkMap[name] = id;       
        idToLandmark[id] = name;      
        searchLookup[normalizeString(name)] = id;
    }
    file.close();
}

// --- 3. Dijkstra's Algorithm ---
void Dijkstras(Node* startNode) {
    for (size_t i = 0; i < allNodes.size(); ++i) {
        allNodes[i]->distanceFromStart = MAX_DISTANCE;
        allNodes[i]->previous = NULL;
    }

    startNode->distanceFromStart = 0;
    
    priority_queue<Node*, vector<Node*>, NodeCompare> pq;
    pq.push(startNode);

    while (!pq.empty()) {
        Node* current = pq.top();
        pq.pop();

        if (current->distanceFromStart == MAX_DISTANCE) continue;

        for (size_t i = 0; i < current->adj.size(); ++i) {
            Edge* edge = current->adj[i];
            Node* neighbor = (edge->node1 == current) ? edge->node2 : edge->node1;
            
            int newDist = current->distanceFromStart + edge->distance;

            if (newDist < neighbor->distanceFromStart) {
                neighbor->distanceFromStart = newDist;
                neighbor->previous = current;
                pq.push(neighbor);
            }
        }
    }
}

// --- 4. Print Result ---
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

// --- UPDATED: Find Closest Service with Search Modes ---
// Mode 0: Simple Substring (Default)
// Mode 1: Whole Word Match (For Bus) - checks for _bus_
// Mode 2: Petrol Logic - checks for 'petrol_pump' OR 'petrol_station'
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
            // Mode 0: Standard substring match (Police, Hospital)
            if (stringContains(normName, serviceKeyword)) isMatch = true;
        } 
        else if (searchMode == 1) {
            // Mode 1: Whole word match (Bus)
            // Ensures "bus_station" matches but "business" does not.
            if (hasWholeWord(normName, serviceKeyword)) isMatch = true;
        }
        else if (searchMode == 2) {
            // Mode 2: Petrol Logic
            // Checks specifically for "petrol_pump" OR "petrol_station"
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

// --- 5. Show Available Landmarks ---
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

// --- Loading Screen ---
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

// --- Main ---
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
            
            // Route Planning Logic
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
            // --- EMERGENCY MENU ---
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

                FindClosestService(userLoc, keyword, 0); // Mode 0: Standard
            }
        }
        else if (choice == 4) {
            // --- PUBLIC TRANSPORT (BUS) ---
            printHeader("Public Transport (Bus)");
            cout << "   Find the nearest Bus Station." << endl;
            cout << "   -----------------------------" << endl;
            string userLoc;
            cout << "   Enter your current location: ";
            getline(cin, userLoc);

            // Mode 1: Search for "bus" with underscores before/after
            FindClosestService(userLoc, "bus", 1); 
        }
        else if (choice == 5) {
            // --- FUEL STATION ---
            printHeader("Fuel Station Locator");
            cout << "   Find the nearest Petrol Pump/Station." << endl;
            cout << "   -------------------------------------" << endl;
            string userLoc;
            cout << "   Enter your current location: ";
            getline(cin, userLoc);

            // Mode 2: Search for "petrol_pump" OR "petrol_station"
            FindClosestService(userLoc, "petrol", 2); 
        }
    }

    return 0;
}