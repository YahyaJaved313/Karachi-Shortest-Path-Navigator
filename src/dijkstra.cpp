#include "dijkstra.h"
#include "globals.h"
#include <queue>

using namespace std;

struct NodeCompare {
    bool operator()(Node* a, Node* b) {
        return a->distanceFromStart > b->distanceFromStart;
    }
};

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