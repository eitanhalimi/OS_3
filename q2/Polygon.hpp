// Polygon.hpp
#pragma once
#include "Point.hpp"
#include <cmath>
#include <iterator>

template <typename Container>
float polygon_area(const Container& poly) {
    float area = 0;
    int n = std::distance(poly.begin(), poly.end());
    if (n < 3) return 0;
    auto it = poly.begin();
    for (int i = 0; i < n; ++i) {
        auto next = it;
        ++next;
        if (next == poly.end()) next = poly.begin();
        area += it->x * next->y;
        area -= it->y * next->x;
        ++it;
    }
    return std::abs(area) / 2.0;
}
