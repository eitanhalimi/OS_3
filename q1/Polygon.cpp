// Polygon.cpp
#include "Polygon.hpp"
#include <cmath>

double polygon_area(const std::vector<Point>& poly) {
    double area = 0;
    int n = poly.size();
    for (int i = 0; i < n; ++i) {
        area += poly[i].x * poly[(i+1)%n].y;
        area -= poly[i].y * poly[(i+1)%n].x;
    }
    return std::abs(area) / 2.0;
}
