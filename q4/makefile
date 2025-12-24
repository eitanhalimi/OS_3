CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11

SERVER_SRCS = server.cpp ../q3/graph.cpp
CLIENT_SRCS = client.cpp
SERVER_TARGET = server.out
CLIENT_TARGET = client.out

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_SRCS)
	$(CXX) $(CXXFLAGS) $(SERVER_SRCS) -o $(SERVER_TARGET)

$(CLIENT_TARGET): $(CLIENT_SRCS)
	$(CXX) $(CXXFLAGS) $(CLIENT_SRCS) -o $(CLIENT_TARGET)

clean:
	rm -f $(SERVER_TARGET) $(CLIENT_TARGET) *.o

.PHONY: all clean
