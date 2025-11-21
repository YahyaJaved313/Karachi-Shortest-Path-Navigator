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
#include <cstdlib> // For system("cls")
#include <iomanip> // For formatting

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

// Clear the console screen (Cross-platform compatible-ish)
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Print a decorative header
void printHeader(string title) {
    clearScreen();
    cout << "============================================================" << endl;
    cout << "   KARACHI NAVIGATION SYSTEM " << endl;
    cout << "============================================================" << endl;
    cout << " > " << title << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << endl;
}

// Print a section divider
void printDivider() {
    cout << "\n------------------------------------------------------------\n";
}

// Normalizer for search
string normalizeString(string input) {
    string result = "";
    for (char c : input) {
        if (c == ' ') result += '_';
        else result += tolower(c);
    }
    return result;
}

// --- Comparator for Dijkstra ---
struct NodeCompare {
    bool operator()(Node* a, Node* b) {
        return a->distanceFromStart > b->distanceFromStart;
    }
};

// --- 1. Graph Creation ---
void createGraph() {
    // A. Load Nodes
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

    // B. Load Edges
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

// --- 4. Print Result (Improved UI) ---
void PrintShortestRouteTo(Node* destination, Node* source, string destName, string sourceName, bool showDetailed) {
    printHeader("Route Calculation Results");

    if (destination->distanceFromStart == MAX_DISTANCE) {
        cout << "\n   [!] ERROR: No path found. Locations are not connected." << endl;
        cout << "\n   Press Enter to return...";
        cin.ignore(); cin.get();
        return;
    }

    // Reconstruct Path
    vector<string> rawPath;
    Node* curr = destination;
    while (curr != NULL) {
        rawPath.push_back(curr->id);
        curr = curr->previous;
    }
    reverse(rawPath.begin(), rawPath.end());

    // Prepare View List
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

    // --- SECTION 1: TRIP SUMMARY ---
    cout << "   TRIP SUMMARY" << endl;
    cout << "   ------------" << endl;
    cout << "   From:       " << sourceName << endl;
    cout << "   To:         " << destName << endl;
    cout << "   Distance:   " << destination->distanceFromStart << " meters" << endl;
    cout << "   Stops:      " << printList.size() << " items shown" << endl; 
    printDivider();

    // --- SECTION 2: THE ROUTE ---
    cout << "   TURN-BY-TURN DIRECTIONS" << endl;
    cout << "   -----------------------" << endl;

    if (printList.empty()) {
        cout << "   (Path is strictly passing through unnamed roads)" << endl;
    } else {
        for (size_t i = 0; i < printList.size(); ++i) {
            // Visual formatting for the list
            if (i == 0) cout << "   (START)  "; 
            else if (i == printList.size() - 1) cout << "   ( END )  ";
            else cout << "      |     ";

            cout << printList[i] << endl;

            // Add a connector line except for the last one
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

// --- 5. Show Available Landmarks (Paginated) ---
void showLandmarks() {
    printHeader("Available Landmarks Repository");
    
    int count = 0;
    int pageSize = 20; // Show 20 at a time

    auto it = landmarkMap.begin();
    while (it != landmarkMap.end()) {
        
        // Print a chunk
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
        
        // Clear screen for next page for cleaner UI
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
        cout << "   [3] Exit System" << endl;
        cout << endl;
        cout << "   Select Option >> ";
        
        int choice;
        if (!(cin >> choice)) { 
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(); // Consume newline

        if (choice == 3) {
            cout << "\n   Exiting... Goodbye!" << endl;
            break;
        }
        else if (choice == 2) {
            showLandmarks();
        }
        else if (choice == 1) {
            string sourceInput, destInput, sourceID, destID;
            
            // --- STEP 1: SELECT SOURCE ---
            while(true) {
                printHeader("Plan Route: Step 1/3");
                cout << "   Enter Start Location (Name): ";
                getline(cin, sourceInput);
                
                string key = normalizeString(sourceInput);
                if (searchLookup.find(key) != searchLookup.end()) {
                    sourceID = searchLookup[key];
                    sourceInput = idToLandmark[sourceID]; // Format nicely
                    break;
                }
                cout << "\n   [!] Error: '" << sourceInput << "' not found. Try again." << endl;
                cout << "   Press Enter to retry...";
                cin.get();
            }

            // --- STEP 2: SELECT DESTINATION ---
            while(true) {
                printHeader("Plan Route: Step 2/3");
                cout << "   From: " << sourceInput << endl;
                cout << "   -----------------------------------" << endl;
                cout << "   Enter Destination (Name): ";
                getline(cin, destInput);
                
                string key = normalizeString(destInput);
                if (searchLookup.find(key) != searchLookup.end()) {
                    destID = searchLookup[key];
                    destInput = idToLandmark[destID]; // Format nicely
                    break;
                }
                cout << "\n   [!] Error: '" << destInput << "' not found. Try again." << endl;
                cout << "   Press Enter to retry...";
                cin.get();
            }

            // --- STEP 3: VIEW PREFERENCE ---
            printHeader("Plan Route: Step 3/3");
            cout << "   From: " << sourceInput << endl;
            cout << "   To:   " << destInput << endl;
            cout << "   -----------------------------------" << endl;
            cout << "   [D] Detailed View (Show Intersection IDs)" << endl;
            cout << "   [L] Landmarks Only (Clean View)" << endl;
            cout << endl;
            cout << "   Select Preference >> ";
            
            char viewType;
            cin >> viewType;
            bool showDetailed = (viewType == 'd' || viewType == 'D');

            // Calculation
            printHeader("Calculating...");
            Node* sourceNode = nodeLookup[sourceID];
            Node* destNode = nodeLookup[destID];

            Dijkstras(sourceNode);
            PrintShortestRouteTo(destNode, sourceNode, destInput, sourceInput, showDetailed);
        }
    }

    return 0;
}