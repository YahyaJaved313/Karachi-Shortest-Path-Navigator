#include "graph.h"
#include "globals.h"
#include "utils.h"
#include <fstream>
#include <iostream>

using namespace std;

void createGraph() {
    ifstream locationsFile("../text_files/karachi_locations.txt");
    if (!locationsFile.is_open()) {
        cerr << "Error: Could not open ../text_files/karachi_locations.txt. Check your folder structure!" << endl;
        return;
    }

    string nodeID;
    while (locationsFile >> nodeID) {
        Node* newNode = new Node(nodeID);
        allNodes.push_back(newNode);
        nodeLookup[nodeID] = newNode; 
    }
    locationsFile.close();

    ifstream roadsFile("../text_files/karachi_roads.txt");
    if (!roadsFile.is_open()) {
        cerr << "Error: Could not open ../text_files/karachi_roads.txt" << endl;
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

void loadLandmarks() {
    ifstream file("../text_files/karachi_landmarks.txt");
    if (!file.is_open()) {
        cerr << "Warning: Could not open ../text_files/karachi_landmarks.txt" << endl;
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