// q3/graph.cpp
#include <list>
#include <sstream>
#include "vector"
#include "graph.hpp"

Point stringToPoint(const std::string& coords) {
    std::stringstream ss(coords);
    std::string xs, ys;
    std::getline(ss, xs, ',');
    std::getline(ss, ys);
    float x = std::stof(xs);
    float y = std::stof(ys);
    return Point(x, y);
}

void newGraph(std::list<Point>& graph, const std::vector<Point>& newPoints) {
    graph.clear();
    for (const auto& p : newPoints) graph.push_back(p);
}

void newPoint(std::list<Point>& graph, const Point& p) {
    graph.push_back(p);
}

void removePoint(std::list<Point>& graph, const Point& p) {
    graph.remove(p);
}

float calcCH(const std::list<Point>& graph) {
    auto ch = convex_hull(graph);
    return polygon_area(ch);
}