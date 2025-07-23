// ConvexHull.hpp
#pragma once
#include <vector>
#include <algorithm>
#include "Point.hpp"

static float cross(const Point& O, const Point& A, const Point& B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

template <typename Container>
std::vector<Point> convex_hull(const Container& input) {
    std::vector<Point> P(input.begin(), input.end());
    int n = P.size(), k = 0;
    if (n <= 2) return P;
    std::sort(P.begin(), P.end());
    std::vector<Point> H(2 * n);

    // Lower hull
    for (int i = 0; i < n; ++i) {
        while (k >= 2 && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
        H[k++] = P[i];
    }
    // Upper hull
    for (int i = n - 2, t = k + 1; i >= 0; --i) {
        while (k >= t && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
        H[k++] = P[i];
    }
    H.resize(k - 1);
    return H;
}
