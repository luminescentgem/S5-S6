#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "graph.hpp"
#include "tools.hpp"
#include <iostream>
#include <unordered_set>
#include <queue>
#include <cassert>

template <class Vertex>
class Solver
{
    const Graph<Vertex> &g;
    std::unordered_map<Vertex, int> independant;
    std::vector<Vertex> vertices;
    std::unordered_map<Vertex, int> dependant;

public:
    Solver(const Graph<Vertex> &_g): 
        g(_g),
        independant()
    {
        {
            std::unordered_set<Vertex> v = g.vertices();
            vertices{v.begin(), v.end()};
        }
        
        for (auto v : vertices) {
            dependant[v] = 0;
        }
    }

    void solve_greedy()
    {
        auto queue = vertices;
        std::sort(queue.begin(), queue.end(),
                  [](Vertex& a, Vertex& b) -> int {
                    return a.degree() - b.degree();
                  });
        
        while(!queue.empty()) {
            Vertex& v = queue.front();

            independant[v] = 1; // No stable neighbor by definition
            update_neighbors(v); // Update the count (or add the neighbor)

            remove_neighbors_from_queue(queue, v);
        }
    }

    // Tries to decrease the size of the dominating set
    bool improve()
    {
        for (auto [v, degree] : dependant) {
            
        }
    }

    // Getter for the solution
    const std::unordered_set<Vertex> &solution()
    {
        return independant;
    }

private:
    void update_neighbors(Vertex& v) const
    {
        for (Vertex n : g.neighbors(v)) {
            dependant[n] += 1;
        }
            
    }

    void remove_neighbors_from_queue(std::vector<Vertex>& list, Vertex& v)
    {
        std::erase(
            std::remove_if(list,
                [&v](Vertex& n) { return g.closedneighbors(v).contains(n); }
            ),
            list.end()
        );
    }

};

// Save the solution to a file
template <class Vertex>
void save(const std::string &outfn, std::unordered_set<Vertex> solution)
{
    std::ofstream outfile(outfn);
    for (Vertex v : solution)
    {
        outfile << v << std::endl;
    }
    std::cout << "Saved a dominating set of size " << solution.size() << std::endl;
}

#endif
