#include "Graph.h"
#include "MinHeap.h"
#include <climits>
#include <algorithm>
#include <iomanip>

using namespace std;

Graph::Graph(int vertices) : numVertices(vertices) {
    cityNames.resize(numVertices);
    adjList.resize(numVertices);
    for (int i = 0; i < numVertices; ++i) {
        cityNames[i] = "Zone_" + to_string(i);
    }
}

void Graph::setCityName(int id, const string& name) {
    if (id >= 0 && id < numVertices) {
        cityNames[id] = name;
    }
}

string Graph::getCityName(int id) const {
    if (id >= 0 && id < numVertices) {
        return cityNames[id];
    }
    return "Unknown_Zone";
}

int Graph::getNumVertices() const {
    return numVertices;
}

void Graph::addEdge(int src, int dest, int weight) {
    if (src >= 0 && src < numVertices && dest >= 0 && dest < numVertices && weight >= 0) {
        adjList[src].push_back({dest, weight});
        adjList[dest].push_back({src, weight});
    }
}

void Graph::printGraph() const {
    cout << "\n--- City Road Network (Adjacency List) ---\n";
    for (int u = 0; u < numVertices; ++u) {
        cout << "[" << u << "] " << cityNames[u] << " -> ";
        if (adjList[u].empty()) {
            cout << "No direct roads";
        } else {
            for (size_t i = 0; i < adjList[u].size(); ++i) {
                const auto& edge = adjList[u][i];
                cout << cityNames[edge.dest] << " (" << edge.weight << " min)";
                if (i + 1 < adjList[u].size()) cout << ", ";
            }
        }
        cout << "\n";
    }
}

void Graph::shortestPathDijkstra(int src, int dest) const {
    if (src < 0 || src >= numVertices || dest < 0 || dest >= numVertices) {
        cout << "[Error] Invalid source or destination zone ID.\n";
        return;
    }

    vector<int> dist(numVertices, INT_MAX);
    vector<int> parent(numVertices, -1);
    MinHeap pq;

    dist[src] = 0;
    pq.push(0, src);

    while (!pq.isEmpty()) {
        pair<int, int> current = pq.pop();
        int d = current.first;
        int u = current.second;

        if (d > dist[u]) continue;

        for (const auto& edge : adjList[u]) {
            int v = edge.dest;
            int weight = edge.weight;

            if (dist[u] != INT_MAX && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
                pq.push(dist[v], v);
            }
        }
    }

    cout << "\n[Dijkstra Shortest Path]: " << cityNames[src] << " -> " << cityNames[dest] << "\n";
    if (dist[dest] == INT_MAX) {
        cout << "No reachable route found between these zones.\n";
        return;
    }

    vector<int> path;
    for (int curr = dest; curr != -1; curr = parent[curr]) {
        path.push_back(curr);
    }
    reverse(path.begin(), path.end());

    cout << "Estimated Travel Time: " << dist[dest] << " mins\n";
    cout << "Green Corridor Route: ";
    for (size_t i = 0; i < path.size(); ++i) {
        cout << cityNames[path[i]] << " [" << path[i] << "]";
        if (i + 1 < path.size()) cout << " ==> ";
    }
    cout << "\n";
}

void Graph::mstPrim() const {
    if (numVertices == 0) return;

    vector<int> key(numVertices, INT_MAX);
    vector<int> parent(numVertices, -1);
    vector<bool> inMST(numVertices, false);
    MinHeap pq;

    key[0] = 0;
    pq.push(0, 0);

    while (!pq.isEmpty()) {
        int u = pq.pop().second;
        if (inMST[u]) continue;
        inMST[u] = true;

        for (const auto& edge : adjList[u]) {
            int v = edge.dest;
            int weight = edge.weight;
            if (!inMST[v] && weight < key[v]) {
                key[v] = weight;
                parent[v] = u;
                pq.push(key[v], v);
            }
        }
    }

    int totalWeight = 0;
    cout << "\n[Prim's Minimum Spanning Tree - Emergency Corridor Backbone]:\n";
    for (int i = 1; i < numVertices; ++i) {
        if (parent[i] != -1) {
            cout << "  " << cityNames[parent[i]] << " --(" << key[i] << " min)--> " << cityNames[i] << "\n";
            totalWeight += key[i];
        }
    }
    cout << "Total Minimum Corridor Cost: " << totalWeight << " mins\n";
}