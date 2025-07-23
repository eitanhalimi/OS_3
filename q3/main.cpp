// q3/main.cpp
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include "../q2/Point.hpp"
#include "graph.cpp" 

// convert "X,Y" to Point
Point stringToPoint(const std::string& coords) {
    std::stringstream ss(coords);
    std::string xs, ys;
    std::getline(ss, xs, ',');
    std::getline(ss, ys);
    float x = std::stof(xs);
    float y = std::stof(ys);
    return Point(x, y);
}

int main() {
    std::list<Point> points;
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        for (auto& c : cmd) c = std::tolower(c);

        if (cmd == "newgraph") {
            int n;
            iss >> n;
            std::vector<Point> newPts;
            for (int i = 0; i < n; ++i) {
                std::getline(std::cin, line); // קרא שורה של נקודה
                newPts.push_back(stringToPoint(line));
            }
            points.clear();
            newGraph(points, newPts);
        }
        else if (cmd == "newpoint") {
            std::string coords;
            iss >> coords;
            newPoint(points, stringToPoint(coords));
        }
        else if (cmd == "removepoint") {
            std::string coords;
            iss >> coords;
            removePoint(points, stringToPoint(coords));
        }
        else if (cmd == "ch") {
            float area = calcCH(points);
            std::cout << area << std::endl;
        }
        else if (cmd == "quit") {
            break;
        }
    }
    return 0;
}