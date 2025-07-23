// q3/main.cpp
#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include "../q2/ConvexHull.hpp"
#include "../q2/Polygon.hpp"

std::string to_lower(const std::string& s) {
    std::string res = s;
    std::transform(res.begin(), res.end(), res.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return res;
}

int main()
{
    std::list<Point> points;

    std::string line;
    while (std::getline(std::cin, line))
    {
        std::string cmd;
        std::istringstream iss(line);
        iss >> cmd;
        cmd = to_lower(cmd);
        float x, y;
        char c;

        if (cmd == "newgraph")
        {
            int n;
            iss >> n;
            points.clear();
            for (int i = 0 ; i < n ; ++i)
            {
                std::cin >> x >> c >> y;
                points.emplace_back(x, y);
            }
        }
        if (cmd == "newpoint" || cmd == "removepoint")
        {
            std::string coords;
            iss >> coords; 

            std::stringstream ss(coords);
            std::string xs, ys;
            std::getline(ss, xs, ','); 
            std::getline(ss, ys);   

            float x = std::stof(xs);
            float y = std::stof(ys);

            if (cmd == "newpoint")
                points.emplace_back(x, y);
            else if (cmd == "removepoint")
                points.remove(Point(x, y));
        }

        if (cmd == "ch")
        {
            auto hull = convex_hull(points);
            float area = polygon_area(hull);
            std::cout << area << std::endl;
        }

        if (cmd == "quit")
        {
            break;
        }
    }

    return 0;
}