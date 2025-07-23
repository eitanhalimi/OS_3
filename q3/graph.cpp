// graph.h
#include <list>
#include "vector"
#include "../q2/Point.hpp"
#include "../q2/ConvexHull.hpp"
#include "../q2/Polygon.hpp"

void newGraph(std::list<Point>& points, const std::vector<Point>& newPoints) {
    points.clear();
    for (const auto& p : newPoints) points.push_back(p);
}

void newPoint(std::list<Point>& points, const Point& p) {
    points.push_back(p);
}

void removePoint(std::list<Point>& points, const Point& p) {
    points.remove(p);
}

float calcCH(const std::list<Point>& points) {
    auto ch = convex_hull(points);
    return polygon_area(ch);
}