// BERTOLINI Garice
#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "graph.hpp"
#include "tools.hpp"
#include <iostream>
#include <unordered_set>
#include <queue>
#include <cassert>
#include <random>
#include <optional>

using Generator = std::mt19937;
using Distributor = std::uniform_int_distribution<std::size_t>;

template <class Vertex>
class Solver
{
    mutable Generator rng;
    
    const Graph<Vertex> &g;
    std::unordered_set<Vertex> independant;
    std::vector<Vertex> vertices;
    std::unordered_map<Vertex, int> dependancy;

public:
    Solver(const Graph<Vertex> &_g):
        rng(std::random_device{}()),
        g(_g),
        independant()
    {
        {
            std::unordered_set<Vertex> v = g.vertices();
            vertices = {v.begin(), v.end()};
        }
        
        for (auto v : vertices) {
            dependancy[v] = 0;
        }
    }

    void solve_greedy()
    {
        auto queue = vertices;
            std::sort(queue.begin(), queue.end(),
                [this](const Vertex& a, const Vertex& b) -> bool {
                    return g.degree(a) < g.degree(b);
                });
        
        while(!queue.empty()) {
            Vertex& v = queue.front();
            independant.insert(v);
            incrementNeighbors(v); // Update the count (or add the neighbor)
            removeNeighborsFromQueue(queue, v);
        }
    }

    // Tries to decrease the size of the dominating set
    bool improve()
    {
        size_t ogSize = independant.size();
        
        removeRandomInd();

        while (auto opt = randomNeighborless()) {
            independant.insert(*opt);
            incrementNeighbors(*opt);
        }
        
        return independant.size() > ogSize;
    }

    // Getter for the solution
    const std::unordered_set<Vertex>& solution()
    {
        return independant;
    }

private:
    void incrementNeighbors(const Vertex& v)
    {
        for (Vertex u : g.closedNeighbors(v)) {
            dependancy[u] += 1;
        }
    }

    void decrementNeighbors(const Vertex& v)
    {
        for (Vertex u : g.closedNeighbors(v)) {
            dependancy[u] -= 1;
        }
    }

    void removeNeighborsFromQueue(std::vector<Vertex>& list, Vertex& v)
    {
        // capture closed by value to avoid binding a reference to a temporary
        auto closed = g.closedNeighbors(v);
        list.erase(
            std::remove_if(list.begin(), list.end(),
                [closed](const Vertex& n) { return closed.contains(n); }
            ),
            list.end()
        );
    }

    std::optional<Vertex> randomNeighborless() const
    {
        std::vector<Vertex> noNeighbors;
        for (auto [v, ind_degree] : dependancy) 
            if (ind_degree == 0)
                noNeighbors.push_back(v);
        
        if (noNeighbors.size() == 0) return {};

        Distributor dis(0, noNeighbors.size() - 1);
        size_t randint = dis(rng);
        return noNeighbors[randint];
    }

    void removeRandomInd()
    {
        if (independant.empty()) return ;

        Distributor dis(0, independant.size() - 1);

        auto it = independant.begin();
        std::advance(it, dis(rng));
        
        decrementNeighbors(*it);

        independant.erase(it);
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
