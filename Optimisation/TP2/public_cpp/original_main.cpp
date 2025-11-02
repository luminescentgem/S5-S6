#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"


template <class Number>
struct Point {
  Number x,y;
  Point(Number _x, Number _y) :x(_x),y(_y) {}
  double distance(Point<Number> p) const {
    return sqrt((x-p.x)*(x-p.x) + (y-p.y)*(y-p.y));
  }
  bool operator==(Point p) const {
    return p.x == x && p.y == y;
  }
};

template <class Number>
class Solver {
  std::vector<Point<Number>> instancePoints;
  Number radius;

public:
  Solver(std::string fn) {
    std::ifstream in(fn, std::ifstream::in | std::ifstream::binary);
    rapidjson::IStreamWrapper isw {in};
    rapidjson::Document doc {};
    doc.ParseStream(isw);

    const rapidjson::Value& jspoints = doc["points"];
    for (auto & jspoint : jspoints.GetArray()) {
      Number x = jspoint["x"].GetDouble();
      Number y = jspoint["y"].GetDouble();
      instancePoints.push_back(Point<Number>{x,y});
    }

    radius = doc["radius"].GetDouble();
    std::cout << "Read " << instancePoints.size() << " points with radius " << radius << "." << std::endl;
  }

  Point<Number> getExtremePoint(std::vector<Point<Number>> candidates, Point<Number> dir) {
    return *std::min_element(candidates.begin(), candidates.end(),
                            [&dir](Point<Number> a, Point<Number> b){
                              return a.x*dir.x + a.y*dir.y < b.x*dir.x + b.y*dir.y;});
  }

  void removeCandidates(std::vector<Point<Number>> &candidates, Point<Number> p) {
    std::erase_if(candidates,
                  [p, this](Point<Number> q){return p.distance(q) <= 2*radius;});
  }

  std::vector<Point<Number>> greedy(Point<Number> dir) {
    std::vector<Point<Number>> solution;
    std::vector<Point<Number>> candidates = instancePoints;

    while(!candidates.empty()) {
      Point<Number> p = getExtremePoint(candidates, dir);
      solution.push_back(p);
      removeCandidates(candidates, p);
    }

    return solution;
  }

  std::vector<Point<Number>> manyRuns(int angles = 8) {
    std::vector<Point<long long int>> bestSolution;
    std::cout << "Found " << angles << " independent sets of size:" << std::flush;

    for(int i = 0; i < angles; i++) {
      double angle = i * 2 * M_PI / angles;
      Point<long long int> dir(65536 * cos(angle), 65536 * sin(angle));
      std::vector<Point<long long int>> solution = greedy(dir);
      std::cout << " " << solution.size() << std::flush;
      if(bestSolution.size() < solution.size()) {
        bestSolution = solution;
        std::cout << "*" << std::flush;
      }
    }
    return bestSolution;
  }

  void writeSolutionSVG(std::string fn, std::vector<Point<Number>> solution, int image_size=1000) {
    Number x0 = std::min_element(instancePoints.begin(), instancePoints.end(),
                                  [](Point<Number> a, Point<Number> b){return a.x < b.x ;}
                                  )->x - radius;
    Number y0 = std::min_element(instancePoints.begin(), instancePoints.end(),
                                  [](Point<Number> a, Point<Number> b){return a.y < b.y ;}
                                  )->y - radius;
    Number x1 = std::max_element(instancePoints.begin(), instancePoints.end(),
                                  [](Point<Number> a, Point<Number> b){return a.x < b.x ;}
                                  )->x + radius;
    Number y1 = std::max_element(instancePoints.begin(), instancePoints.end(),
                                  [](Point<Number> a, Point<Number> b){return a.y < b.y ;}
                                  )->y + radius;
    Number input_size = std::max(x1-x0, y1-y0);
    double image_radius = (double) image_size * radius / input_size;

    auto inputToImagePoint = [x0,y1,image_size,input_size](Point<Number> p) {
      Point<double> q((double)(p.x - x0) * image_size / input_size,
                      (double)(y1 - p.y) * image_size / input_size);
      return q;
    };

    std::ofstream fsvg(fn);
    fsvg << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
    Point<double> image_size_xy = inputToImagePoint(Point{x1,y0});
    fsvg << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"" << image_size_xy.x << "\" height=\"" << image_size_xy.y << "\">" << std::endl;

    for(auto input_p : instancePoints) {
      if(std::find(solution.begin(), solution.end(), input_p) == solution.end()) {
        Point<double> image_p = inputToImagePoint(input_p);
        fsvg << " <circle stroke=\"black\" fill=\"none\" stroke-width=\"2\" cx=\"" << image_p.x << "\" cy=\"" << image_p.y << "\" r=\"" << image_radius << "\">" << std::endl;
        fsvg << "  <title>(" << input_p.x << "," << input_p.y << ")</title>" << std::endl;
        fsvg << " </circle>" << std::endl;
      }
    }

    for(auto input_p : solution) {
      Point<double> image_p = inputToImagePoint(input_p);
      fsvg << " <circle stroke=\"blue\" fill=\"none\" stroke-width=\"2\" cx=\"" << image_p.x << "\" cy=\"" << image_p.y << "\" r=\"" << image_radius << "\">" << std::endl;
      fsvg << "  <title>(" << input_p.x << "," << input_p.y << ")</title>" << std::endl;
      fsvg << " </circle>" << std::endl;
    }

    fsvg << "</svg>" << std::endl;
  }
};


int main(int argc, char **argv) {
  Solver<long long int> solver(argv[1]);

  std::vector<Point<long long int>> solution = solver.manyRuns();

  std::cout << std::endl << "Best: " << solution.size() << std::endl;
  solver.writeSolutionSVG(argv[2], solution);

  return 0;
}
