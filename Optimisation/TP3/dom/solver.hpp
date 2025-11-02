#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "graph.hpp"
#include "tools.hpp"
#include <iostream>
#include <unordered_set>
#include <queue>
#include <cassert>


template<class Vertex>
class Solver {
  const Graph<Vertex> &g;
  std::unordered_set<Vertex> dominating, not_dominating, dominated, not_dominated;
  std::vector<Vertex> vertices;
  std::priority_queue<std::pair<int,Vertex>> heap;

public:
  Solver(const Graph<Vertex> &_g) :
    g(_g),
    not_dominating(g.vertices()),
    not_dominated(not_dominating),
    vertices(not_dominating.begin(), not_dominating.end()) {
  }

  // Insert a vertex v in the dominating set, updating all sets accordingly
  void insert_dominating(Vertex v) {
    dominating.insert(v);
    not_dominating.erase(v);
    // Neighbors of v are now dominated
    for(Vertex u : g.closedNeighbors(v)) {
      dominated.insert(u);
      not_dominated.erase(u);
    }
  }

  // Tests if a vertex v is dominated using only dominating
  // Needed to update dominated on erase_dominating
  bool test_dominated(Vertex v) const {
    for(Vertex u : g.closedNeighbors(v)) {
      if(dominating.contains(u))
        return true;
    }
    return false;
  }

  // Insert a vertex v in the dominating set, updating all sets accordingly
  // Returns false if the vertex was not in the dominating set
  bool erase_dominating(Vertex v) {
    if(!dominating.contains(v))
      return false;
    dominating.erase(v);
    not_dominating.insert(v);
    for(Vertex u : g.closedNeighbors(v)) {
      if(!test_dominated(u)) {
        dominated.erase(u);
        not_dominated.insert(u);
      }
    }
    return true;
  }

  // Counts the number of vertices that would change into dominated if v is inserted into dominating
  int count_domination(Vertex v) const {
    int ret = 0;
    for(Vertex u : g.closedNeighbors(v)) {
      if(!dominated.contains(u))
        ret++;
    }

    return ret;
  }

  Vertex choose_vertex() {
    std::vector<Vertex> best_vertices;
    int best_value = 0;

    while(!heap.empty() &&
        (best_vertices.empty() || heap.top().first == best_value)) {
      auto [old_val,v] = heap.top();
      heap.pop();
      if (!dominating.contains(v)) {
        int cur_val = count_domination(v);
        if(old_val == cur_val) {
          best_vertices.push_back(v);
          best_value = cur_val;
        }
        else {
          auto p = std::make_pair(cur_val,v);
          heap.push(p);
        }
      }
    }

    assert(!best_vertices.empty());

    Vertex ret = random_element(best_vertices);
    for(Vertex v : best_vertices) {
      if(v != ret) {
        auto p = std::make_pair(best_value,v);
        heap.push(p);
      }
    }

    return ret;
  }


  void solve_greedy() {
    for(Vertex v : not_dominating) {
      auto p = std::make_pair(count_domination(v),v);
      heap.push(p);
    }

    while(not_dominated.size()) {
      Vertex v = choose_vertex();
      insert_dominating(v);
    }
  }

  bool would_dominate_all(const std::vector<Vertex> &dom) const {
    std::unordered_set<Vertex> neighbors;
    for(Vertex v : dom) {
      auto nv = g.closedNeighbors(v);
      neighbors.insert(nv.begin(),nv.end());
    }
    for(Vertex v : not_dominated) {
      if(!neighbors.contains(v))
        return false;
    }
    return true;
  }

  // Very slow function to find the smallest set of vertices to insert to get a dominating set
  // Only efficient if very few vertices need to be inserted
  // A known solution is given as a parameter
  void solve_exact(const std::unordered_set<long long int> &removed) {
    std::unordered_set<Vertex> candidates;

    for(Vertex v :not_dominated) {
      auto nv = g.closedNeighbors(v);
      candidates.insert(nv.begin(), nv.end());
    }

    for(size_t sz = 0; sz <= removed.size(); sz++) {
      std::vector<std::vector<Vertex>> comb = combinations(candidates, sz);
      std::shuffle(comb.begin(), comb.end(), rgen);
      for(std::vector<Vertex> dom : comb) {
        if(would_dominate_all(dom)) {
          for(Vertex v : dom)
            insert_dominating(v);
          return;
        }
      }
    }

    assert(false);
  }


  // Tries to decrease the size of the dominating set
  // Returns true when succesful
  // It will never increase the size
  bool improve() {
    size_t previous = dominating.size();
    Vertex v = random_element(vertices);
    int count = 0;
    std::unordered_set<Vertex> removed;
    for(Vertex u : g.closedNeighbors(v)) {
      if(erase_dominating(u)) {
        count++;
        removed.insert(u);
      }
      if(count > 3) { // The exact solver is too slow to handle more than 4
        break;
      }
    }

    solve_exact(removed);
    return dominating.size() < previous;
  }


  // Getter for the solution
  const std::unordered_set<Vertex> &solution() {
    return dominating;
  }
};

// Save the solution to a file
template<class Vertex>
void save(const std::string &outfn, std::unordered_set<Vertex> solution) {
  std::ofstream outfile(outfn);
  for(Vertex v : solution) {
    outfile << v << std::endl;
  }
  std::cout << "Saved a dominating set of size " << solution.size() << std::endl;
}


#endif
