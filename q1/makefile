CXX = g++
CXXFLAGS = -Wall -std=c++17

OBJS = main.o ConvexHull.o Polygon.o

convex_hull: $(OBJS)
	$(CXX) $(CXXFLAGS) -o convex_hull $(OBJS)

main.o: main.cpp Point.hpp ConvexHull.hpp Polygon.hpp
ConvexHull.o: ConvexHull.cpp ConvexHull.hpp Point.hpp
Polygon.o: Polygon.cpp Polygon.hpp Point.hpp

clean:
	rm -f *.o convex_hull
