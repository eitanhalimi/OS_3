// main.cpp
#include <iostream>
#include <vector>
#include <iomanip>
#include "Point.hpp"
#include "ConvexHull.hpp"
#include "Polygon.hpp"
#include <algorithm>

int main() {
    int n;
    std::cin >> n;
    // std::cin.ignore(); // חשוב לדלג על השורה הריקה אחרי הקריאה ל־n
    std::vector<Point> points(n);
    for (int i = 0; i < n; ++i) {
        std::string line;
        std::getline(std::cin, line);
        std::replace(line.begin(), line.end(), ',', ' '); // מחליף פסיק ברווח
        std::istringstream iss(line);
        iss >> points[i].x >> points[i].y;
    }
    auto hull = convex_hull(points);
    double area = polygon_area(hull);
    std::cout << area << std::endl;
    return 0;
}
