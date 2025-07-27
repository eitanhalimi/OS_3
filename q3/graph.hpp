// q3/graph.hpp
#include <list>
#include <sstream>
#include "vector"
#include "../q2/Point.hpp"
#include "../q2/ConvexHull.hpp"
#include "../q2/Polygon.hpp"

// convert "X,Y" to Point
Point stringToPoint(const std::string& coords);

// create a new graph
void newGraph(std::list<Point>& graph, const std::vector<Point>& newPoints);

// add a point to an existing graph
void newPoint(std::list<Point>& graph, const Point& p);

// remove a point from a graph
void removePoint(std::list<Point>& graph, const Point& p);

// return area of graph's convex hull
float calcCH(const std::list<Point>& graph);