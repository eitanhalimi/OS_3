# Makefile for q7 multi-threaded server

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread

SERVER_SRC = server.cpp ../q3/graph.cpp
CLIENT_SRC = client.cpp
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)
SERVER_TARGET = server.out
CLIENT_TARGET = client.out

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(CLIENT_TARGET): $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER_OBJ) $(CLIENT_OBJ) $(SERVER_TARGET) $(CLIENT_TARGET)

.PHONY: all clean
