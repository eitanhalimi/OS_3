CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11

Q2 = ../q2
Q3 = ../q3
Q4 = .

SRCS = $(Q4)/server.cpp $(Q3)/graph.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = ch_server.out

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(Q4)/*.o $(Q3)/*.o

.PHONY: all clean
