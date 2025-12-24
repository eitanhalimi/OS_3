CXX = g++
FLAGS = -Wall -g -std=c++20 -fPIC -pthread
LIB_TARGET = lib_q8.so
EXEC_TARGET = server.out
CLIENT_TARGET = client.out
GRAPH_OBJ = ../q3/graph.o

all: $(LIB_TARGET) $(EXEC_TARGET) $(CLIENT_TARGET)

$(LIB_TARGET): reactor.o proactor.o
	$(CXX) -shared $^ -o $@

$(EXEC_TARGET): server.o reactor.o proactor.o $(GRAPH_OBJ)
	$(CXX) $(FLAGS) $^ -o $@

$(CLIENT_TARGET): client.o
	$(CXX) $(FLAGS) $^ -o $@

reactor.o: reactor.cpp reactor.hpp
	$(CXX) $(FLAGS) -c $< -o $@

proactor.o: proactor.cpp proactor.hpp
	$(CXX) $(FLAGS) -c $< -o $@

server.o: server.cpp proactor.hpp ../q3/graph.hpp
	$(CXX) $(FLAGS) -c $< -o $@

client.o: client.cpp
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -f *.o $(LIB_TARGET) $(EXEC_TARGET) $(CLIENT_TARGET) *.out

.PHONY: all clean
