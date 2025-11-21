#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>
#include <vector>
#include <limits>

using namespace std;

// Global constant for infinity
const int MAX_DISTANCE = numeric_limits<int>::max();

class Node; // Forward declaration

class Edge {
public:
    Node* node1;
    Node* node2;
    int distance;
    Edge(Node* n1, Node* n2, int dist) : node1(n1), node2(n2), distance(dist) {}
};

class Node {
public:
    string id;
    int distanceFromStart;
    Node* previous;
    vector<Edge*> adj; 

    Node(string id) : id(id), distanceFromStart(MAX_DISTANCE), previous(NULL) {}
};

#endif