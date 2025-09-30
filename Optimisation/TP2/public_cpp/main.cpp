#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <tuple>
#include <functional>
#include <numeric>

namespace std
{
    template <>
    struct hash<std::tuple<int, int>>
    {
        std::size_t operator()(const std::tuple<int, int> &t) const
        {
            auto h1 = std::hash<int>{}(std::get<0>(t));
            auto h2 = std::hash<int>{}(std::get<1>(t));
            return h1 ^ (h2 << 1);
        }
    };
}
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

template <class Number>
struct Point
{
    Number x, y;
    Point(Number _x, Number _y) : x(_x), y(_y) {}
    double distance(Point<Number> p) const
    {
        return sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y));
    }
    double distance2(Point<Number> p) const
    {
        return (x - p.x) * (x - p.x) + (y - p.y) * (y - p.y);
    }
    bool operator==(Point p) const
    {
        return p.x == x && p.y == y;
    }
};

template <class Number>
class Solver
{
    std::vector<Point<Number>> pts;
    Number radius;
    
    using Cell = Point<long long>;

    struct CellHash {
        size_t operator()(const Cell& c) const noexcept {
            // Usual mix
            size_t h1 = std::hash<long long>{}(c.x);
            size_t h2 = std::hash<long long>{}(c.y);
            return h1 ^ (h2 << 1);
        }
    };

    // Build cell key with floor to handle negatives
    static Cell key_of(const Point<Number> &p, Number cellSize)
    {
        long double L = static_cast<long double>(cellSize);
        long double fx = std::floor(static_cast<long double>(p.x) / L);
        long double fy = std::floor(static_cast<long double>(p.y) / L);
        return {static_cast<long long>(fx), static_cast<long long>(fy)};
    }

public:
    Solver(std::string fn)
    {
        // Open file : O(1)
        std::ifstream in(fn, std::ifstream::in | std::ifstream::binary);
        rapidjson::IStreamWrapper isw{in};
        rapidjson::Document doc{};
        doc.ParseStream(isw);

        // Load points in a vector : O(n)
        const rapidjson::Value &jspoints = doc["points"];
        for (auto &jspoint : jspoints.GetArray())
        {
            Number x = jspoint["x"].GetDouble();
            Number y = jspoint["y"].GetDouble();
            pts.push_back(Point<Number>{x, y});
        }

        // Load radius value : O(1)
        radius = doc["radius"].GetDouble();
        std::cout << "Read " << pts.size()
                  << " points with radius " << radius
                  << "." << std::endl;
    }

    std::vector<Point<Number>> greedy(Point<Number> dir)
    {
        // List of indices
        std::vector<int> indexes(pts.size());
        std::iota(indexes.begin(), indexes.end(), 0); // range(0, n)
        
        // Sort the indexes depending on dir
        auto proj = [&](int i) {
            return pts[i].x * dir.x + pts[i].y * dir.y;
        };
        std::sort(indexes.begin(), indexes.end(), /*cmp*/[&](int i, int j) {
            return proj(i) < proj(j);
        });

        // Instantiate a grid of indexes of point
        const Number cellSize = Number(2) * radius;
        std::unordered_map<Cell, std::vector<int>, CellHash> grid;
        
        // Match each point with a cell on a grid
        int ptsSize = (int) pts.size();
        for (int i = 0; i < ptsSize; ++i) {
            grid[key_of(pts[i], cellSize)].push_back(i);
        }

        std::vector<uint8_t> alive(pts.size(), 1);

        // Lambda that kills p's neighbours using its index (ip)
        auto kill_neighbours = [&](int ip) //! O(n)
        {
            const Number dist_max2 = Number(4) * radius * radius;
            const auto &p = pts[ip]; // Fetch p
            
            // Get cell_x and cell_y which is p / cellSize
            auto [cx, cy] = key_of(p, cellSize);

            // Search around the target cell with a delta betwen -1 and 1
            for (long long dx = -1; dx <= 1; ++dx)
                for (long long dy = -1; dy <= 1; ++dy)
                {
                    // Find cell with binary search 
                    auto it = grid.find({cx + dx, cy + dy}); //! O(log n)
                    if (it == grid.end())
                        continue;
                    
                    // Iterate through all neighbours
                    for (int j : it->second) //! O(n)
                    {
                        if (!alive[j]) // ignore if dead
                            continue;
                        
                        // Kill using squared distance for efficiency
                        if (p.distance2(pts[j]) <= dist_max2)
                            alive[j] = 0; // will kill p eventually
                    }
                }
        };

        std::vector<Point<Number>> solution;
        solution.reserve(pts.size()); // ensure no reallocation is necessary

        // Main algorithm, idea is unchanged but should now be O(n²)
        for (int k = (int) indexes.size() - 1; k >= 0; --k) //! O(n²)
        {
            int i = indexes[k];
            if (!alive[i])  // ignore dead 
                continue;
            solution.push_back(pts[i]);
            kill_neighbours(i); //! O(n)
        }
        return solution;
    }

    std::vector<Point<Number>> manyRuns(int angles = 8)
    {
        std::vector<Point<Number>> bestSolution;
        std::cout << "Found " << angles
                  << " independent sets of size:" << std::flush;

        for (int i = 0; i < angles; ++i)
        {
            double angle = i * 2 * M_PI / angles;
            Point<long long int> dir(65536 * cos(angle), 65536 * sin(angle));
            std::vector<Point<long long int>> solution = greedy(dir);
            std::cout << " " << solution.size() << std::flush;
            if (bestSolution.size() < solution.size())
            {
                bestSolution = solution;
                std::cout << "*" << std::flush;
            }
        }
        return bestSolution;
    }

    void writeSolutionSVG(std::string fn, std::vector<Point<Number>> solution,
                          int image_size = 1000)
    {
        Number x0 = std::min_element(pts.begin(), pts.end(),
                                     [](Point<Number> a, Point<Number> b)
                                     { return a.x < b.x; })
                        ->x -
                    radius;
        Number y0 = std::min_element(pts.begin(), pts.end(),
                                     [](Point<Number> a, Point<Number> b)
                                     { return a.y < b.y; })
                        ->y -
                    radius;
        Number x1 = std::max_element(pts.begin(), pts.end(),
                                     [](Point<Number> a, Point<Number> b)
                                     { return a.x < b.x; })
                        ->x +
                    radius;
        Number y1 = std::max_element(pts.begin(), pts.end(),
                                     [](Point<Number> a, Point<Number> b)
                                     { return a.y < b.y; })
                        ->y +
                    radius;
        Number input_size = std::max(x1 - x0, y1 - y0);
        double image_radius = (double)image_size * radius / input_size;

        auto inputToImagePt = [x0, y1, image_size, input_size](Point<Number> p)
        {
            Point<double> q((double)(p.x - x0) * image_size / input_size,
                            (double)(y1 - p.y) * image_size / input_size);
            return q;
        };

        std::ofstream fsvg(fn);
        fsvg << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
        Point<double> image_size_xy = inputToImagePt(Point{x1, y0});
        fsvg << "<svg xmlns=\"http://www.w3.org/2000/svg\""
             << " version=\"1.1\" width=\""
             << image_size_xy.x
             << "\" height=\""
             << image_size_xy.y
             << "\">"
             << std::endl;

        for (auto input_p : pts)
        {
            auto it = std::find(solution.begin(), solution.end(), input_p);
            if (it == solution.end()) {
                Point<double> image_p = inputToImagePt(input_p);
                fsvg << " <circle"
                     << " stroke=\"black\""
                     << " fill=\"none\""
                     << " stroke-width=\"2\""
                     << " cx=\""<< image_p.x << "\""
                     << " cy=\"" << image_p.y << "\""
                     << " r=\"" << image_radius << "\""
                     << ">" << std::endl;

                fsvg << "  <title>"
                     << "(" << input_p.x << "," << input_p.y << ")"
                     << "</title>" << std::endl;

                fsvg << " </circle>" << std::endl;
            }
        }

        for (auto input_p : solution)
        {
            Point<double> image_p = inputToImagePt(input_p);
            fsvg << " <circle"
                 << " stroke=\"blue\""
                 << " fill=\"none\""
                 << " stroke-width=\"2\""
                 << " cx=\"" << image_p.x << "\" cy=\"" << image_p.y << "\""
                 << " r=\"" << image_radius << "\""
                 << ">" << std::endl;

            fsvg << "  <title>"
                 << "(" << input_p.x << "," << input_p.y << ")"
                 << "</title>" << std::endl;
                 
            fsvg << " </circle>" << std::endl;
        }

        fsvg << "</svg>" << std::endl;
    }
};

int main(int argc, char **argv)
{
    Solver<long long int> solver(argv[1]);

    std::vector<Point<long long int>> solution = solver.manyRuns();

    std::cout << std::endl
              << "Best: " << solution.size() << std::endl;
    solver.writeSolutionSVG(argv[2], solution);

    return 0;
}
