// ConvexHull.cpp
#include "ConvexHull.hpp"
#include <algorithm>

// Cross product
static float cross(const Point& O, const Point& A, const Point& B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

std::vector<Point> convex_hull(const std::vector<Point>& P) {
    int n = P.size(), k = 0;
    if (n <= 2) return P;
    std::vector<Point> points = P; // Make a copy to sort
    std::sort(points.begin(), points.end());
    std::vector<Point> H(2 * n);

    // Lower hull
    for (int i = 0; i < n; ++i) {
        while (k >= 2 && cross(H[k-2], H[k-1], points[i]) <= 0) k--;
        H[k++] = points[i];
    }
    // Upper hull
    for (int i = n - 2, t = k + 1; i >= 0; --i) {
        while (k >= t && cross(H[k-2], H[k-1], points[i]) <= 0) k--;
        H[k++] = points[i];
    }
    H.resize(k - 1);
    return H;
}
