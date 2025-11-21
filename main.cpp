#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <map>
#include <unordered_map>
#include <queue>
#include <algorithm>

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
map<string, string> landmarkMap;            // Name -> ID (for searching)
unordered_map<string, string> idToLandmark; // ID -> Name (for printing route)

// --- Helper: Comparator for Priority Queue ---
struct NodeCompare {
    bool operator()(Node* a, Node* b) {
        return a->distanceFromStart > b->distanceFromStart;
    }
};

// --- 1. Graph Creation ---
void createGraph() {
    cout << "Loading graph data..." << endl;
    
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
    cout << "Loaded " << allNodes.size() << " locations (nodes)." << endl;

    // B. Load Edges
    ifstream roadsFile("karachi_roads.txt");
    if (!roadsFile.is_open()) {
        cerr << "Error: Could not open karachi_roads.txt" << endl;
        return;
    }

    string u, v;
    int dist;
    int edgeCount = 0;

    while (roadsFile >> u >> v >> dist) {
        if (nodeLookup.find(u) != nodeLookup.end() && nodeLookup.find(v) != nodeLookup.end()) {
            Node* n1 = nodeLookup[u];
            Node* n2 = nodeLookup[v];
            Edge* newEdge = new Edge(n1, n2, dist);
            n1->adj.push_back(newEdge);
            n2->adj.push_back(newEdge);
            edgeCount++;
        }
    }
    roadsFile.close();
    cout << "Loaded " << edgeCount << " roads (edges)." << endl;
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
    }
    file.close();
    cout << "Loaded " << landmarkMap.size() << " landmarks." << endl;
}

// --- 3. Dijkstra's Algorithm ---
void Dijkstras(Node* startNode) {
    // Reset all nodes
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
    if (destination->distanceFromStart == MAX_DISTANCE) {
        cout << "\n[!] No path found. These locations might be disconnected." << endl;
        return;
    }

    // 1. Reconstruct full path (IDs)
    vector<string> rawPath;
    Node* curr = destination;
    while (curr != NULL) {
        rawPath.push_back(curr->id);
        curr = curr->previous;
    }
    reverse(rawPath.begin(), rawPath.end());

    // 2. Prepare the List to Print based on User Choice
    vector<string> printList;
    
    for (size_t i = 0; i < rawPath.size(); ++i) {
        string id = rawPath[i];
        bool isLandmark = (idToLandmark.find(id) != idToLandmark.end());

        if (showDetailed) {
            // DETAILED: Add everything. If it's a landmark, show name, else show ID.
            if (isLandmark) {
                printList.push_back(idToLandmark[id]);
            } else {
                printList.push_back(id);
            }
        } else {
            // LANDMARKS ONLY: Only add if it is a landmark.
            if (isLandmark) {
                printList.push_back(idToLandmark[id]);
            }
        }
    }

    // 3. Print Output
    cout << "\n============================================" << endl;
    cout << "              ROUTE DETAILS                 " << endl;
    cout << "============================================" << endl;
    cout << "From:   " << sourceName << endl;
    cout << "To:     " << destName << endl;
    cout << "Type:   " << (showDetailed ? "Detailed (All Intersections)" : "Landmarks Only") << endl;
    cout << "--------------------------------------------" << endl;
    
    if (printList.empty()) {
        cout << "(Path exists, but no named landmarks lie on this route)" << endl;
    } else {
        int stepCount = 0;
        for (size_t i = 0; i < printList.size(); ++i) {
            cout << "[" << printList[i] << "]";

            if (i != printList.size() - 1) {
                cout << " --> ";
                stepCount++;
                // Newline for readability
                if (stepCount % 4 == 0) cout << "\n      "; 
            }
        }
    }
    
    cout << "\n--------------------------------------------" << endl;
    cout << "Total Distance:       " << destination->distanceFromStart << " meters" << endl;
    cout << "Total Nodes Passed:   " << rawPath.size() << endl;
    cout << "============================================" << endl;
}

// --- 5. Show Available Landmarks ---
void showLandmarks() {
    cout << "\n--- Available Landmarks (Top 50 shown) ---" << endl;
    int count = 0;
    for(map<string, string>::iterator it = landmarkMap.begin(); it != landmarkMap.end(); ++it) {
        cout << it->first << "  "; 
        count++;
        if(count % 3 == 0) cout << endl;
        if(count >= 50) {
            cout << "\n... and " << landmarkMap.size() - 50 << " more." << endl;
            break;
        }
    }
    cout << endl;
}

// --- Main ---
int main() {
    createGraph();
    loadLandmarks();

    if (allNodes.empty()) {
        cout << "Error: Graph is empty." << endl;
        cout << "Press Enter to exit...";
        cin.get();
        return 1;
    }

    int choice;
    do {
        cout << "\n--- Menu ---" << endl;
        cout << "1. Find Shortest Path" << endl;
        cout << "2. List Available Landmarks" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter choice: ";
        
        if (!(cin >> choice)) { 
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (choice == 2) {
            showLandmarks();
        }
        else if (choice == 1) {
            string sourceInput, destInput;
            
            // 1. Get Source
            cout << "\nEnter Source Name: ";
            cin >> sourceInput;
            if (landmarkMap.find(sourceInput) == landmarkMap.end()) {
                cout << "Error: '" << sourceInput << "' not found." << endl;
                continue;
            }

            // 2. Get Destination
            cout << "Enter Destination Name: ";
            cin >> destInput;
            if (landmarkMap.find(destInput) == landmarkMap.end()) {
                cout << "Error: '" << destInput << "' not found." << endl;
                continue;
            }

            // 3. ASK USER PREFERENCE (The requested feature)
            char viewType;
            cout << "\n-----------------------------------------" << endl;
            cout << "How would you like to view the route?" << endl;
            cout << "d - Detailed (Show every road intersection ID)" << endl;
            cout << "l - Landmarks Only (Show only English names)" << endl;
            cout << "-----------------------------------------" << endl;
            cout << "Enter choice (d/l): ";
            cin >> viewType;

            bool showDetailed = (viewType == 'd' || viewType == 'D');

            // 4. Perform Calculation
            Node* sourceNode = nodeLookup[landmarkMap[sourceInput]];
            Node* destNode = nodeLookup[landmarkMap[destInput]];

            if (sourceNode && destNode) {
                cout << "\nComputing shortest path..." << endl;
                Dijkstras(sourceNode);
                PrintShortestRouteTo(destNode, sourceNode, destInput, sourceInput, showDetailed);
            } else {
                cout << "Error: Node data missing." << endl;
            }
        }
    } while (choice != 3);

    return 0;
}