#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <thread>
#include <unordered_set>

class Point {
public:
  int x;
  int y;

  Point(int x_, int y_) : x(x_), y(y_) {}

  bool operator==(const Point& p) const { return x == p.x && y == p.y; }
  void min(const Point& p);
  void max(const Point& p);
};

void Point::min(const Point& p) {
  x = std::min(x, p.x);
  y = std::min(y, p.y);
}

void Point::max(const Point& p) {
  x = std::max(x, p.x);
  y = std::max(y, p.y);
}

class PointHash {
  std::hash<int> intHash;

public:
  size_t operator()(const Point& p) const { return intHash(p.x) ^ intHash(p.y); }
};

typedef std::unordered_set<Point, PointHash> Grid;

void inputCells(std::ifstream& in, Grid& grid) {
  int x, y;
  while (in >> x >> y) {
    grid.insert(Point(x, y));
  }
}

int countNeighbors(const Grid& grid, const Point& p) {
  int neighbors = 0;
  for (int dx = -1; dx <= 1; ++dx) {
    for (int dy = -1; dy <= 1; ++dy) {
      if (dx == 0 && dy == 0) continue;

      if (grid.find(Point(p.x + dx, p.y + dy)) != grid.end()) {
        ++neighbors;
      }
    }
  }
  return neighbors;
}

void printGrid(const Grid& grid) {
  if (grid.empty()) return;

  Point min = *grid.begin();
  Point max = min;

  for (Grid::const_iterator it = grid.begin(); it != grid.end(); ++it) {
    min.min(*it);
    max.max(*it);
  }

  for (int y = min.y; y <= max.y; ++y) {
    for (int x = min.x; x <= max.x; ++x) {
      std::cout << (grid.find(Point(x, y)) != grid.end() ? "█" : "░");
    }
    std::cout << std::endl;
  }
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "USAGE: " << argv[0] << " <input file>" << std::endl;
    return 1;
  }

  std::ifstream in(argv[1]);
  if (!in.good()) {
    std::cerr << "ERROR: could not open input file \"" << argv[1] << "\""
              << std::endl;
    return 1;
  }

  Grid grid;
  inputCells(in, grid);

  while (!grid.empty()) {
    printGrid(grid);

    auto begin = std::chrono::system_clock::now();
    
    Grid newGrid;
    for (Grid::const_iterator it = grid.begin(); it != grid.end(); ++it) {
      for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
          Point p(it->x + dx, it->y + dy);
          int neighbors = countNeighbors(grid, p);
          bool alive = neighbors == 3 || (grid.find(p) != grid.end() && neighbors > 1 && neighbors < 4);
          if (alive) newGrid.insert(p);
        }
      }
    }
    grid = newGrid;

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - begin;
    std::cout << "generation time = " << duration.count() << " s" << std::endl;

    std::chrono::milliseconds rate(100);
    if (duration < rate) {
      std::this_thread::sleep_for(rate - duration);
    }

    std::cout << std::endl;
  }
}
