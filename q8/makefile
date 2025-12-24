CXX = g++
FLAGS = -Wall -g -std=c++20 -fPIC
TARGET = libreactorproactor.so

all: $(TARGET) 

$(TARGET): reactor.o proactor.o
	$(CXX) -shared $^ -o $@

reactor.o: reactor.cpp reactor.hpp
	$(CXX) $(FLAGS) -c $< -o $@

proactor.o: proactor.cpp proactor.hpp
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET) *.out

.PHONY: all clean
