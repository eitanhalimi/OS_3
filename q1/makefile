CXX = g++
CXXFLAGS = -Wall -std=c++17
TARGET = main.out

OBJS = main.o ConvexHull.o Polygon.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp Point.hpp ConvexHull.hpp Polygon.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

ConvexHull.o: ConvexHull.cpp ConvexHull.hpp Point.hpp
	$(CXX) $(CXXFLAGS) -c ConvexHull.cpp -o ConvexHull.o

Polygon.o: Polygon.cpp Polygon.hpp Point.hpp
	$(CXX) $(CXXFLAGS) -c Polygon.cpp -o Polygon.o

clean:
	rm -f *.o $(TARGET)

.PHONY: clean
