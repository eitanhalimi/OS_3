CXX = g++
FLAGS = -Wall -g -std=c++20 -fPIC
LIB_TARGET = libreactor.so
EXEC_TARGET = server.out
CLIENT_TARGET = client.out
GRAPH_OBJ = ../q3/graph.o

all: $(LIB_TARGET) $(EXEC_TARGET) $(CLIENT_TARGET)

$(LIB_TARGET): reactor.o
	$(CXX) -shared $< -o $@

reactor.o: reactor.cpp reactor.hpp
	$(CXX) $(FLAGS) -c $< -o $@

$(EXEC_TARGET): server.o reactor.o $(GRAPH_OBJ)
	$(CXX) $(FLAGS) $^ -o $@

$(CLIENT_TARGET): client.o
	$(CXX) $(FLAGS) $^ -o $@

server.o: server.cpp reactor.hpp ../q3/graph.hpp
	$(CXX) $(FLAGS) -c $< -o $@

client.o: client.cpp
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -f *.o $(LIB_TARGET) $(EXEC_TARGET) $(CLIENT_TARGET) *.out

.PHONY: all clean
