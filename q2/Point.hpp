// Point.hpp
#pragma once

struct Point {
    float x, y;
    
    Point(float x=0, float y=0) : x(x), y(y) {}

    bool operator<(const Point& other) const {
        return x < other.x || (x == other.x && y < other.y);
    }

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};
