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
map<string, string> landmarkMap;         

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
        cerr << "Error: Could not open karachi_locations.txt. Make sure it is in the same folder!" << endl;
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
    }
    file.close();
    cout << "Loaded " << landmarkMap.size() << " landmarks." << endl;
}

// --- 3. Dijkstra's Algorithm ---
void Dijkstras(Node* startNode) {
    // Reset all nodes (Standard C++11 loop)
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

        // Standard C++11 loop for neighbors
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
void PrintShortestRouteTo(Node* destination, Node* source, string destName, string sourceName) {
    if (destination->distanceFromStart == MAX_DISTANCE) {
        cout << "\n[!] No path found. These locations might be disconnected." << endl;
        return;
    }

    vector<string> path;
    Node* curr = destination;
    while (curr != NULL) {
        path.push_back(curr->id);
        curr = curr->previous;
    }

    cout << "\n============================================" << endl;
    cout << "       ROUTE CALCULATION COMPLETE           " << endl;
    cout << "============================================" << endl;
    cout << "From:   " << sourceName << endl;
    cout << "To:     " << destName << endl;
    cout << "--------------------------------------------" << endl;
    cout << "Total Distance:       " << destination->distanceFromStart << " meters" << endl;
    cout << "Intersections Passed: " << path.size() - 1 << endl;
    cout << "============================================" << endl;
}

// --- 5. Show Available Landmarks (FIXED LOOP) ---
void showLandmarks() {
    cout << "\n--- Available Landmarks (Top 50 shown) ---" << endl;
    int count = 0;
    
    // FIXED: Using standard iterator instead of C++17 structure binding
    for(map<string, string>::iterator it = landmarkMap.begin(); it != landmarkMap.end(); ++it) {
        cout << it->first << "  "; // Print Name
        
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
        cout << "Error: Graph is empty. Please check text files." << endl;
        // Use a simple pause for Windows users so they can see the error
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
        if (!(cin >> choice)) { // Fix infinite loop if user enters letter
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (choice == 2) {
            showLandmarks();
        }
        else if (choice == 1) {
            string sourceInput, destInput;
            cout << "\nEnter Source Name (e.g. Jinnah_International_Airport): ";
            cin >> sourceInput;
            
            if (landmarkMap.find(sourceInput) == landmarkMap.end()) {
                cout << "Error: '" << sourceInput << "' not found." << endl;
                continue;
            }

            cout << "Enter Destination Name: ";
            cin >> destInput;

            if (landmarkMap.find(destInput) == landmarkMap.end()) {
                cout << "Error: '" << destInput << "' not found." << endl;
                continue;
            }

            string sourceID = landmarkMap[sourceInput];
            string destID = landmarkMap[destInput];

            if (nodeLookup.find(sourceID) != nodeLookup.end() && nodeLookup.find(destID) != nodeLookup.end()) {
                Node* sourceNode = nodeLookup[sourceID];
                Node* destNode = nodeLookup[destID];

                cout << "\nComputing shortest path..." << endl;
                Dijkstras(sourceNode);
                PrintShortestRouteTo(destNode, sourceNode, destInput, sourceInput);
            } else {
                cout << "Error: Internal ID mismatch." << endl;
            }
        }
    } while (choice != 3);

    return 0;
}




























// #include <iostream>
// #include <vector>
// #include <string>
// #include <fstream>  // For file I/O
// #include <limits>   // For INT_MAX

// using namespace std;

// // --- Forward Declarations ---
// class Node;
// class Edge;

// // --- Global Variables ---
// vector<Node*> nodes; // Stores all nodes
// vector<Edge*> edges; // Stores all edges
// int totDistance;     // Stores the total distance for the last query

// // Use INT_MAX for infinity, which is safer than a magic number
// const int MAX_DISTANCE = numeric_limits<int>::max();

// // --- Node Class ---
// // Represents a location (a node in the graph)
// class Node {
// public:
//     string id;
//     Node* previous;
//     int distanceFromStart;

//     // Constructor to initialize the node
//     Node(string id) : id(id), previous(NULL), distanceFromStart(MAX_DISTANCE) {
//         // We will add the node to the global 'nodes' vector inside createGraph()
//     }
// };

// // --- Edge Class ---
// // Represents a road (an edge in the graph)
// class Edge {
// public:
//     Node* node1;
//     Node* node2;
//     int distance;

//     // Constructor to initialize the edge
//     Edge(Node* n1, Node* n2, int dist) : node1(n1), node2(n2), distance(dist) {
//         // We will add the edge to the global 'edges' vector inside createGraph()
//     }

//     // Helper function to check if this edge connects two specific nodes
//     bool Connects(Node* n1, Node* n2) {
//         return (
//             (n1 == this->node1 && n2 == this->node2) ||
//             (n1 == this->node2 && n2 == this->node1)
//         );
//     }
// };

// // --- Helper Function to Find a Node by ID ---
// Node* findNode(const string& id) {
//     for (Node* node : nodes) {
//         if (node->id == id) {
//             return node;
//         }
//     }
//     return nullptr; // Not found
// }

// // --- Graph Creation Function ---
// // Reads from the text files to populate the 'nodes' and 'edges' vectors
// void createGraph() {
//     ifstream locationsFile("karachi_locations.txt");
//     string locationName;

//     // 1. Read all locations (nodes)
//     if (!locationsFile.is_open()) {
//         cerr << "Error: Could not open karachi_locations.txt" << endl;
//         return;
//     }
//     while (locationsFile >> locationName) {
//         Node* newNode = new Node(locationName);
//         nodes.push_back(newNode);
//     }
//     locationsFile.close();
//     cout << "Loaded " << nodes.size() << " locations." << endl;

//     // 2. Read all roads (edges)
//     ifstream roadsFile("karachi_roads.txt");
//     string name1, name2;
//     int dist;

//     if (!roadsFile.is_open()) {
//         cerr << "Error: Could not open karachi_roads.txt" << endl;
//         return;
//     }
//     int edgeCount = 0;
//     while (roadsFile >> name1 >> name2 >> dist) {
//         Node* n1 = findNode(name1);
//         Node* n2 = findNode(name2);

//         if (n1 != nullptr && n2 != nullptr) {
//             Edge* newEdge = new Edge(n1, n2, dist);
//             edges.push_back(newEdge);
//             edgeCount++;
//         } else {
//             cerr << "Warning: Could not find nodes for edge: " << name1 << " <-> " << name2 << endl;
//         }
//     }
//     roadsFile.close();
//     cout << "Loaded " << edgeCount << " roads." << endl;
// }

// // --- Dijkstra's Algorithm Helper Functions ---

// // Finds the node with the smallest distance in the 'unvisited' list,
// // removes it from the list, and returns it.
// Node* ExtractSmallest(vector<Node*>& unvisited) {
//     int size = unvisited.size();
//     if (size == 0) return NULL;

//     int smallestPosition = 0;
//     Node* smallest = unvisited.at(0);

//     for (int i = 1; i < size; ++i) {
//         Node* current = unvisited.at(i);
//         if (current->distanceFromStart < smallest->distanceFromStart) {
//             smallest = current;
//             smallestPosition = i;
//         }
//     }

//     unvisited.erase(unvisited.begin() + smallestPosition);
//     return smallest;
// }

// // Checks if a node is present in the 'unvisited' list
// bool Contains(vector<Node*>& unvisited, Node* node) {
//     const int size = unvisited.size();
//     for (int i = 0; i < size; ++i) {
//         if (node == unvisited.at(i)) {
//             return true;
//         }
//     }
//     return false;
// }

// // Returns all nodes adjacent to 'node' which are still in the 'unvisited' collection.
// vector<Node*>* AdjacentRemainingNodes(Node* node, vector<Node*>& unvisited) {
//     vector<Node*>* adjacentNodes = new vector<Node*>();
//     const int size = edges.size();
//     for (int i = 0; i < size; ++i) {
//         Edge* edge = edges.at(i);
//         Node* adjacent = NULL;

//         if (edge->node1 == node) {
//             adjacent = edge->node2;
//         } else if (edge->node2 == node) {
//             adjacent = edge->node1;
//         }

//         if (adjacent && Contains(unvisited, adjacent)) {
//             adjacentNodes->push_back(adjacent);
//         }
//     }
//     return adjacentNodes;
// }

// // Returns the distance between two directly connected nodes
// int Distance(Node* node1, Node* node2) {
//     const int size = edges.size();
//     for (int i = 0; i < size; ++i) {
//         Edge* edge = edges.at(i);
//         if (edge->Connects(node1, node2)) {
//             return edge->distance;
//         }
//     }
//     return -1; // Should not happen in a connected graph
// }

// // --- Main Dijkstra's Algorithm Function ---
// // This function will run the algorithm. It operates on a *copy*
// // of the nodes list so it doesn't destroy the graph.
// void Dijkstras() {
//     // Create a copy of the global nodes vector to serve as the unvisited set
//     vector<Node*> unvisitedNodes = nodes;

//     while (unvisitedNodes.size() > 0) {
//         Node* smallest = ExtractSmallest(unvisitedNodes);

//         // If smallest is NULL or has MAX_DISTANCE, we can stop
//         if(smallest == NULL || smallest->distanceFromStart == MAX_DISTANCE) {
//             break;
//         }

//         vector<Node*>* adjacentNodes = AdjacentRemainingNodes(smallest, unvisitedNodes);

//         const int size = adjacentNodes->size();
//         for (int i = 0; i < size; ++i) {
//             Node* adjacent = adjacentNodes->at(i);
//             int distance = Distance(smallest, adjacent) + smallest->distanceFromStart;

//             if (distance < adjacent->distanceFromStart) {
//                 adjacent->distanceFromStart = distance;
//                 adjacent->previous = smallest;
//             }
//         }
//         delete adjacentNodes;
//     }
// }

// // --- Path Printing Function ---
// // Traces back from the destination to print the path and distance
// void PrintShortestRouteTo(Node* destination, Node* source) {
//     if (destination->distanceFromStart == MAX_DISTANCE) {
//         cout << "No path found from " << source->id << " to " << destination->id << "." << endl;
//         return;
//     }
    
//     vector<string> route;
//     Node* previous = destination;

//     // Store total distance
//     totDistance = destination->distanceFromStart;
//     cout << "\nDistance from " << source->id << " to " << destination->id << ": "
//          << totDistance << "km" << endl;

//     // Trace the path backwards
//     while (previous) {
//         route.push_back(previous->id);
//         previous = previous->previous;
//     }

//     // Print the path in the correct (forward) order
//     cout << "\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\n";
//     cout << "Shortest Path:\n";
//     for (auto i = route.crbegin(); i != route.crend(); ++i) {
//         cout << *i;
//         if (i + 1 != route.crend()) {
//             cout << " -> ";
//         }
//     }
//     cout << "\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\n";
// }

// // --- Helper to Reset Nodes for a New Query ---
// void resetNodes() {
//     for (Node* node : nodes) {
//         node->distanceFromStart = MAX_DISTANCE;
//         node->previous = nullptr;
//     }
// }


// // --- Main Function ---
// int main() {
//     // 1. Load the graph data from files
//     createGraph();

//     if (nodes.empty() || edges.empty()) {
//         cerr << "Graph is empty. Please check your .txt files. Exiting." << endl;
//         return 1;
//     }

//     cout << "\nWelcome to the Karachi Path Navigator!" << endl;

//     int choice;
//     do {
//         cout << "\n--- Menu ---" << endl;
//         cout << "1. Find Shortest Path" << endl;
//         cout << "2. Exit" << endl;
//         cout << "Enter your choice: ";
//         cin >> choice;

//         if (choice == 1) {
//             // Reset all node distances for the new query
//             resetNodes();

//             string sourceName, destName;
//             cout << "\nEnter source place: ";
//             cin >> sourceName;
//             cout << "Enter destination place: ";
//             cin >> destName;

//             Node* sourceNode = findNode(sourceName);
//             Node* destNode = findNode(destName);

//             if (sourceNode == nullptr) {
//                 cout << "Error: Source location '" << sourceName << "' not found." << endl;
//                 continue;
//             }
//             if (destNode == nullptr) {
//                 cout << "Error: Destination location '" << destName << "' not found." << endl;
//                 continue;
//             }

//             // Set source distance to 0
//             sourceNode->distanceFromStart = 0;

//             // Run Dijkstra's
//             Dijkstras();

//             // Print the result
//             PrintShortestRouteTo(destNode, sourceNode);
//         }

//     } while (choice != 2);

//     cout << "Thank you for using the Karachi Path Navigator!" << endl;

//     // --- Cleanup ---
//     // (In a real application, you'd delete all new'd Nodes and Edges)
//     // For this simple console app, we'll let the OS clean up.

//     return 0;
// }