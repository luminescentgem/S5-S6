#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <vector>
#include <queue>
#include <cassert>

template<class Vertex>
class Graph {
  std::unordered_map<Vertex, std::unordered_set<Vertex>> adj;
  
public:
  Graph() {
  }  
  void addVertex(Vertex v) {
    adj[v];
  }
  
  Graph(std::string filename) {
    std::ifstream infile(filename);
    Vertex u,v;
    while(infile >> u >> v) {
      addEdge(u,v);
    }
  }

  void addEdge(Vertex u, Vertex v) {
    if(u != v) {
      adj[u].insert(v);
      adj[v].insert(u);
    }
  }
  
  bool containsVertex(Vertex v) const {
    return adj.count(v) != 0;
  }
  
  bool containsEdge(Vertex u, Vertex v) const {
    return containsVertex(u) && adj.at(u).count(v) != 0;
  }
  
  int degree(Vertex v) const {
    return containsVertex(v) ? adj.at(v).size() : -1;
  }
  
  int maxDegree() const {
    int ret = -1;
    for(const auto &[v, neigh] : adj) {
      ret = std::max(ret,(int)neigh.size());
    }
    
    return ret;
  }
  
  int countVertices() const {
    return adj.size();
  }
  
  int countEdges() const {
    int ret = 0;
    for(const auto &[v, neigh] : adj) {
      ret += neigh.size();
    }
    assert(ret % 2 == 0);
    return ret / 2;
  }
  
  void removeEdge(Vertex u, Vertex v) {
    if(containsEdge(u,v)) {
      adj.at(u).erase(v);
      adj.at(v).erase(u);
    }
  }
  
  void removeVertex(Vertex v) {
    if(containsVertex(v)) {
      std::unordered_set<Vertex> neigh(adj.at(v)); // Copy because changing the data structure invalidades the iterator
      for(Vertex u : neigh) {
        removeEdge(u,v);
      }
      adj.erase(v);
    }
  }

  void clear() {
    adj.clear();
  }
  
  std::unordered_set<Vertex> vertices() const {
    std::unordered_set<Vertex> ret;
    for(const auto &[v, neigh] : adj) {
      ret.insert(v);
    }
    return ret;
  }
  
  const std::unordered_set<Vertex> &neighbors(Vertex v) const {
    return adj.at(v);
  }

  // Neighbors including v itself
  std::unordered_set<Vertex> closedNeighbors(Vertex v) const {
    std::unordered_set<Vertex> ret = neighbors(v);
    ret.insert(v);
    return ret;
  }
  
  std::vector<Vertex> bfs(Vertex v, int maxv = 0) const {
    std::unordered_set<Vertex> visited;
    std::vector<Vertex> ret;
    std::queue<Vertex> fifo;
    
    if(maxv == 0)
      maxv = countVertices();
    
    fifo.push(v);
    while(!fifo.empty() && ret.size() < (size_t) maxv) {
      Vertex u = fifo.front();
      fifo.pop();
      if(visited.count(u) != 0)
        continue;
      ret.push_back(u);
      visited.insert(u);
      for(Vertex w : neighbors(u))
        fifo.push(w);
    }
    
    return ret;
  }
};


#endif
