# Makefile for q7 multi-threaded server

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread

SRC = server.cpp ../q3/graph.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = server_q7.out

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
