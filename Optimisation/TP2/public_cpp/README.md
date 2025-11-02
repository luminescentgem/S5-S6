# Optimisation - TP1
*BERTOLINI Garice*

## **Base code sumary**
Here is a compact and simplified version of the base code, explaining the important components.

### A. Types
```c++
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
```
```c++
using PointList = std::vector<Point>
```

### B. Class Properties
```c++
class Solver {
  PointList instancePoints;     // all circle centers
  Number radius;                // every circle's radius
}
```

### C. Main Algorithm

```c++
PointList greedy(Point dir) {   // runs 8 times
    PointList solution;
    PointList candidates = instancePoints;// O(n)

    while(!candidates.empty()) {          // O(n²)
      Point p = getExtremePoint(candidates, dir);   // O(n)  
      solution.push_back(p);                        // O(1)
      removeCandidates(candidates, p);              // O(n)
    }

    return solution;
  }
```

```c++
Point<Number> getExtremePoint(PointList& candidates, Point dir) {
    return *std::min_element(   // O(n)
        candidates.begin(), candidates.end(),
        [&dir](Point a, Point b) {
            return a.x*dir.x + a.y*dir.y < b.x*dir.x + b.y*dir.y;
        }
    );
}
```

```c++
void removeCandidates(PointList& candidates, Point p) {
    std::erase_if(  // O(n)
        candidates,
        [p, this](Point<Number> q) {
            return p.distance(q) <= 2*radius;   // O(1)
        }
    );
}
```

```c++
PointList manyRuns(int angles = 8) {    // O(8n²)
    PointList bestSolution;

    for(int i = 0; i < angles; i++) {   // O(8) -> O(1)
        double angle = i * 2 * PI / angles;         // O(1)               
        Point dir(cos(angle), 65536 * sin(angle));  // O(1)

        PointList solution = greedy(dir);           // O(n²)

        if(bestSolution.size() < solution.size())   // O(1)
            bestSolution = solution;
    }
    return bestSolution;
}
```

# Optimisations

Below, you will find a list of the optimisations I made to the base code, sorted by date.
At first, I tried to "solve" it on my own, before reading the Python version.

*NOTE: THE SOURCE CODE HAS BEEN CAREFULLY COMMENTED!*

## 1. **Sorted `candidates` with `dir` projection:**

The base code iterates through every candidate to find the next point to add to `solution`, and this every iteration of `greedy`:

```c++
while(!candidates.empty()) {    // ~n times
    Point p = getExtremePoint(candidates, dir); // O(n)
    solution.push_back(p);
    removeCandidates(candidates, p);
}
```

```c++
Point<Number> getExtremePoint(PointList& candidates, Point dir) {
    return *std::min_element(   // O(n)
        candidates.begin(), candidates.end(),
        [&dir](Point a, Point b) {
            return a.x*dir.x + a.y*dir.y < b.x*dir.x + b.y*dir.y;
        }
    );
}
```

This is an **O(n)** function in an **O(n)** loop, which is very costy.

Might as well sort `candidates` before the loop.

```c++
std::sort(      // O(n)
    indexes.begin(), indexes.end(),
    [&](Point p, Point q) {
        return p.x * dir.x + p.y * dir.y < q.x * dir.x + q.y * dir.y;
    }
);

while(!candidates.empty()) {    // ~n times
    Point p = candidates.back() // O(1)
    solution.push_back(p);
    removeCandidates(candidates, p);
}
```
We could also opt for `p = candidates[0]` ; but it would mean deleting the first element everytime. Might as well delete the last element to save some time on vector reallocation.

## 2. Dichotomic search to find neighbours faster (OUTDATED)

When deleting a circle's neighbours, the base code compares its center position to every other's, checking if their distance is smaller than `2 * radius`.

This makes it **O(n)**, it even makes it **Ω(n)** if `n` is candidates' size.

```c++
void removeCandidates(PointList& candidates, Point p) {
    std::erase_if(  // O(n)
        candidates,
        [p, this](Point q) {
            return p.distance(q) <= 2*radius;   // O(1)
        }
    );
}
```

The tricky part is that it allows the use of `erase_if`, an efficient method that waits until the loop is over to adjust all remaining elements.

Without it, we might erase elements one by one, thus shifting the whole vector everytime.

Still, dichotomy should save time, especially on large values of n. We do need to pass `dir` as a parameter though, since we need a way to indicate to the binary search how to compare elements.

```c++
void removeNeighbours(PointList& candidates,
                      const Point& p,
                      const Point& dir)
{
    auto projection = [&](const Point& a) -> Number {
        return a.x*dir.x + a.y*dir.y;
    };
    const Number pfar = projection(p);
    const Number pmin = pfar - 2 * radius * 65536;
    const Number pmax = pfar + 2 * radius * 65536;

    auto first = std::lower_bound(candidates.begin(), candidates.end(), pmin,
        [&](const Point& a, const Number& s){ return projection(a) < s; });

    auto last  = std::upper_bound(first, candidates.end(), pmax,
        [&](const Number& s, const Point& b){ return s < projection(b); });

    last = std::remove_if(first, last, [&](const Point& q){
        Number dx = q.x - p.x;
        Number dy = q.y - p.y;
        return dx*dx + dy*dy <= 4*radius*radius;
    });
    candidates.erase(last, candidates.end());
}
```

We can find the hypothetical furthest point (backward and forward) possible in the radius, in the given `dir` projection.

These can be found with binary searches, and their iterator will allow for a tighter and more accurate loop.

Unfortunately, `erase_if` only works on full vectors, not iterators. We will use `remove_if` instead, which swaps soft-deleted elements to the end of the vector, and returns the iterator to the first soft-deleted element for an easy deletion handling.

## 3. Neighbour Grid

We would like some kind of geometrical structure, one that can handle 2D space better.

The Python code uses a grid system that stores circles in cells. It's a map, with coordinates as key, and a vector of Point as value.

To ensure O(1) assignment and access from key to values, it is meant to use Euclydian division on each coordinate, with the cell size as the divider.

In our example, each grid has a `radius * 2` size, ensuring a circle's neighbour will always be in its cell or an adjacant cell.

With `radius = 3`, `Point(16, 21)` will have the key `(16//6, 21//6)`, which is `(2, 3)`. This means it's in the same cell as every point `Point([12:17], [18:23])`, and in adjacent cells to every point `Point([6:23], [12:29])`.

Finding every neighbour then consists of looking up in all 9 adjacant cells, basically O(1), then looking through every point in those cells, which unfortunately still counts as O(n) by default, but is way less.

Here's how it's implemented:

```c++
using Cell = Point;
static Cell key_of(const Point& p, Number cellSize)
{
    int fx = std::floor(p.x / cellSize);
    int fy = std::floor(p.y / cellSize);
    return {fx, fy};
}
```

```c++
class Solver {
    PointList instancePoints;
    std::unordered_map<Cell, std::vector<Point>, CellHash> grid;

    Solver(std::string fn) {
        ...
        for (int i = 0; i < instancePoints.size(); ++i) {
            Point p = instancePoints[i]
            grid[key_of(p, cellSize)].push_back(p);
        }
        ...
    }
}
```

```c++
void removeCandidates(PointList& candidates, Point p) {
    const Number thr2 = 4 * radius * radius;
    auto [ix, iy] = key_of(p);
    
    for (int dx=-1; dx<=1; ++dx) {
    for (int dy=-1; dy<=1; ++dy) {
        
        auto it = grid.find({ix+dx, iy+dy});
        if (it == grid.end()) continue;

        auto& vec = it->second;

        auto it = std::remove_if(
            vec.begin(), vec.end(),
            [&](const Point& q) {
                Number ux = q.x - p.x
                Number uy = q.y - p.y;
                return ux*ux + uy*uy <= thr2;
            }
        );
        
        vec.erase(it, vec.end());
        if (vec.empty()) grid.erase(it);
    }}
}
```

There still is a deletion issue. This leads me to want to try something with flags and indexes instead, because manipulating the vector is too problematic.

## 4. Look Up Grid

Manipulating points directly is too problematic, I want to manipulate indexes instead.

Here's my final (simplified) implementation.

```c++
std::vector<Point<Number>> greedy(Point<Number> dir)
{
    // List of indices
    std::vector<int> indexes(pts.size());
    std::iota(indexes.begin(), indexes.end(), 0); // range(0, n)
    
    // Sort the indexes depending on dir
    auto proj = [&](int i) {
        return pts[i].x * dir.x + pts[i].y * dir.y;
    };
    std::sort(indexes.begin(), indexes.end(), [&](int i, int j) {
        return proj(i) < proj(j);
    });

    // Instantiate a grid of indexes of point
    const Number cellSize = 2 * radius;
    std::unordered_map<Cell, std::vector<int>, CellHash> grid;
    
    // Match each point with a cell on a grid
    for (int i = 0; i < pts.size(); ++i) {
        grid[key_of(pts[i], cellSize)].push_back(i);
    }

    std::vector<bool> alive(pts.size(), 1);

    // Lambda that kills p's neighbours using its index (ip)
    auto kill_neighbours = [&](int ip) //! O(n)
    {
        const Number dist_max2 = 4 * radius * radius;
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

    PointList solution;
    solution.reserve(pts.size()); // ensure no reallocation is necessary

    // Main algorithm, complexity is unchanged but should now be way faster!
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
```
It mainly switches the `Grid<Point>` for a `Grid<int>` where each `int` is an index of a `Point` in `instancePoints` (renamed to `pts`).

It uses a `vector<int>` to store indexes and sort them once.

It also uses a `vector<int>` to track Points that are alive, and by extension, those who have been removed, without having to manipulate the `pts` vector.

There is no need to actually delete anything, but it **does** use more memory, about O(4n).

This also means there is no need for a `candidates` vector, or any copy of `pts`.

# More possible optimisations

## Threads

Threads are not an optimisation per say, but they would definitely reduce the time whil using more performance. It should in theory divide the time `manyRuns` take by 8 (or the amount of angles).

# Limits

Right now, loading and writing take more time than the main algorithm. Any further optimisations are futile.