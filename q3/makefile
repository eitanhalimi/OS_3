CXX = g++
CXXFLAGS = -Wall -std=c++17

TARGET = main.out
OBJS = main.o graph.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp graph.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

graph.o: graph.cpp graph.hpp
	$(CXX) $(CXXFLAGS) -c graph.cpp -o graph.o

clean:
	rm -f *.o $(TARGET)

.PHONY: all clean
