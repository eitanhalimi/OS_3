// ConvexHull_list.cpp
#include <iostream>
#include <list>
#include "ConvexHull.hpp"
#include "Polygon.hpp"

int main() {
    int n;
    std::cin >> n;
    std::cin.ignore();
    std::list<Point> points;

    // Input
    for (int i = 0; i < n; ++i) {
        float x, y;
        char c;
        std::cin >> x >> c >> y;
        points.emplace_back(x, y);
    }

    // Compute convex hull
    auto hull = convex_hull(points);
    float area = polygon_area(hull);
    std::cout << area << std::endl;

    return 0;
}
