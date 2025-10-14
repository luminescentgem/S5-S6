// Guilherme Dias da Fonseca
#include <iostream>
#include "graph.hpp"
#include "solver.hpp"
#include "tools.hpp"

using namespace std;
using Vertex = long long int;

double maxtime = 120;

int main(int argc, char **argv) {
  if(argc != 2) {
    cout << "./main <inputfile>" << endl;
    exit(1);
  }

  Graph<Vertex> g(argv[1]); // Read input graph
  cout << "Read input graph with " << g.countVertices() << " vertices and "
                                   << g.countEdges() << " edges" << endl;

  std::unordered_set<Vertex> solution;

  while(elapsed() < maxtime) {
    int iterations = 0;
    Solver<Vertex> solver(g);
    
    solver.solve_greedy();

    std::cout << "Dominating set size: "
              << solver.solution().size()
              << std::flush;
    
    double improved = elapsed();

    while(elapsed() < maxtime && elapsed() - improved < maxtime / 8) {
      if(solver.improve()) {
        std::cout << " -> " << solver.solution().size() << std::flush;
        improved = elapsed();
      }
      iterations++;
    }

    if(solution.empty() || solver.solution().size() < solution.size())
      solution = solver.solution();
    
    std::cout << std::endl << "After " << iterations << " iterations, we found a dominating set of size " << solver.solution().size() << " and the best size found is " << solution.size() << std::endl;
  }

  string outfn = argv[1]; // Create filename for output
  outfn.replace(outfn.end()-5, outfn.end(), "dom");
  save(outfn, solution);
  
  return 0;
}
