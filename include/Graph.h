#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <string>

struct Edge {
    int dest;
    int weight;
};

class Graph {
private:
    int numVertices;
    std::vector<std::string> cityNames;
    std::vector<std::vector<Edge>> adjList;

public:
    explicit Graph(int vertices);
    void setCityName(int id, const std::string& name);
    std::string getCityName(int id) const;
    int getNumVertices() const;
    void addEdge(int src, int dest, int weight);
    void printGraph() const;
    void shortestPathDijkstra(int src, int dest) const;
    void mstPrim() const;
};

#endif