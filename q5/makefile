CXX = g++
FLAGS = -Wall -g -std=c++20 -fPIC
TARGET = libreactor.so

all: $(TARGET)

$(TARGET): reactor.o
	$(CXX) -shared $< -o $@

reactor.o: reactor.cpp reactor.hpp
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -f *.o  $(TARGET) *.out

.PHONY: all clean
