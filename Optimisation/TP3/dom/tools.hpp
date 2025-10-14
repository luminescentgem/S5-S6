#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <algorithm>
#include <unordered_set>
#include <random>
#include <chrono>

static auto beginTime = std::chrono::high_resolution_clock::now();

inline double elapsed() {
  auto end = std::chrono::high_resolution_clock::now();
  auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - beginTime);

  return dur.count() / 1000.0;
}

static std::mt19937 rgen(1);

// Returns a random element from a vector
template<class T>
T random_element(const std::vector<T> &v) {
  std::uniform_int_distribution<> distrib2(0,v.size()-1);
  return v[distrib2(rgen)];
}


// Returns a vectors with all the subsets of s that have size k
template<class T>
std::vector<std::vector<T>> combinations(const std::unordered_set<T> &s, int k) {
  std::vector<std::vector<T>> previous, ret;
  ret.push_back(std::vector<T>());

  std::vector<T> sv(s.begin(), s.end());
  std::shuffle(sv.begin(), sv.end(), rgen);

  for(int i = 0; i < k; i++) {
    previous = ret;
    ret.clear();
    for(T k : sv) {
      for(auto ps : previous) {
        if(ps.empty() || ps.back() < k) {
          ps.push_back(k);
          ret.push_back(ps);
        }
      }
    }
  }

  return ret;
}

#endif
