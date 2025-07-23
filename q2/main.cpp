// main.cpp
#include <iostream>
#include <list>
#include <deque>
#include <vector>
#include <chrono>
#include "ConvexHull.hpp"
#include "Polygon.hpp"

int main() {
    int n;
    std::cin >> n;
    std::cin.ignore();
    std::list<Point> points_list;
    std::deque<Point> points_deque;

    // Input
    for (int i = 0; i < n; ++i) {
        float x, y;
        char c;
        std::cin >> x >> c >> y;
        points_list.emplace_back(x, y);
        points_deque.emplace_back(x, y);
    }

    // Profile list
    auto t1 = std::chrono::high_resolution_clock::now();
    auto hull_list = convex_hull(points_list);
    auto t2 = std::chrono::high_resolution_clock::now();
    float area_list = polygon_area(hull_list);
    std::cout << "List time:   " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << " us, area: " << area_list << std::endl;

    // Profile deque
    auto t3 = std::chrono::high_resolution_clock::now();
    auto hull_deque = convex_hull(points_deque);
    auto t4 = std::chrono::high_resolution_clock::now();
    float area_deque = polygon_area(hull_deque);
    std::cout << "Deque time:  " << std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count() << " us, area: " << area_deque << std::endl;

    return 0;
}
