# Makefile for q6 server with reactor

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

SRC = server.cpp ../q5/reactor.cpp ../q3/graph.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = server_q6.out

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
