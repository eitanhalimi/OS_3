// q6/server.cpp
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <netinet/in.h>
#include <unistd.h>
#include <unordered_map>
#include "..//q3/graph.hpp"
#include "../q5/reactor.hpp"

#define PORT 9034        // the port client will be connecting to
#define BACKLOG 10       // how many pending connections queue will hold
#define MAXDATASIZE 1024 // max number of bytes we can get at once

// globals
Reactor reactor;
std::list<Point> points;

// client state for ongoing 'newgraph' command
std::unordered_map<int, int> remaining_points;
std::unordered_map<int, std::vector<Point>> partial_points;

// helper, for debug
std::string getGraph(std::list<Point> graph)
{
    std::string s = "the graph is:\n";
    for (Point p : graph)
    {
        s += "(" + std::to_string(p.x) + "," + std::to_string(p.y) + ")\n";
    }
    return s;
}

void handle_client_input(int client_fd)
{
    char buffer[MAXDATASIZE];
    ssize_t valread = recv(client_fd, buffer, MAXDATASIZE - 1, 0);
    if (valread <= 0)
    {
        reactor.removeFd(client_fd);
        close(client_fd);
        remaining_points.erase(client_fd);
        partial_points.erase(client_fd);
        std::cout << "Client " + std::to_string(client_fd) + " disconnected" << std::endl;
        return;
    }

    buffer[valread] = '\0';
    std::string input = buffer;
    input.erase(std::remove(input.begin(), input.end(), '\r'), input.end());

    // Case: ongoing newgraph input
    if (remaining_points.count(client_fd) > 0)
    {
        input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());
        partial_points[client_fd].push_back(stringToPoint(input));
        remaining_points[client_fd]--;

        if (remaining_points[client_fd] == 0)
        {
            newGraph(points, partial_points[client_fd]);
            std::string reply = "Graph updated\n";
            send(client_fd, reply.c_str(), reply.length(), 0);
            remaining_points.erase(client_fd);
            partial_points.erase(client_fd);
        }
        else
        {
            std::string msg = "Enter point X,Y:\n";
            send(client_fd, msg.c_str(), msg.length(), 0);
        }
        return;
    }

    std::istringstream iss(input);
    std::string line;
    while (std::getline(iss, line))
    {
        if (line.empty())
            continue;

        std::istringstream liness(line);
        std::string cmd;
        liness >> cmd;
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

        if (cmd == "newgraph")
        {
            int n = 0;
            liness >> n;
            if (n <= 0)
            {
                std::string msg = "Invalid number of points\n";
                send(client_fd, msg.c_str(), msg.length(), 0);
                return;
            }
            remaining_points[client_fd] = n;
            partial_points[client_fd] = {};
            std::string msg = "Enter point X,Y:\n";
            send(client_fd, msg.c_str(), msg.length(), 0);
        }
        else if (cmd == "newpoint")
        {
            std::string coords;
            liness >> coords;
            newPoint(points, stringToPoint(coords));
            std::string reply = "Point added\n";
            send(client_fd, reply.c_str(), reply.length(), 0);
        }
        else if (cmd == "removepoint")
        {
            std::string coords;
            liness >> coords;
            removePoint(points, stringToPoint(coords));
            std::string reply = "Point removed\n";
            send(client_fd, reply.c_str(), reply.length(), 0);
        }
        else if (cmd == "ch")
        {
            float area = calcCH(points);
            std::ostringstream oss;
            oss << area << "\n";
            send(client_fd, oss.str().c_str(), oss.str().length(), 0);
        }
        else if (cmd == "quit")
        {
            reactor.removeFd(client_fd);
            close(client_fd);
            remaining_points.erase(client_fd);
            partial_points.erase(client_fd);
            std::cout << "Client " + std::to_string(client_fd) + " disconnected" << std::endl;
            return;
        }

        // for debug
        else if (cmd == "getgraph")
        {
            std::string sGraph = getGraph(points);
            send(client_fd, sGraph.c_str(), sGraph.length(), 0);
        }
        else
        {
            std::string msg = "Unknown command\n";
            send(client_fd, msg.c_str(), msg.length(), 0);
        }
    }
}

void handle_new_connection(int server_fd)
{
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
    if (client_fd < 0)
    {
        perror("accept");
        return;
    }

    std::cout << "Client " + std::to_string(client_fd) + " connected\n";
    std::string reply = "Connected to " + std::to_string(PORT) + "\n";
    send(client_fd, reply.c_str(), reply.length(), 0);

    reactor.addFd(client_fd, handle_client_input);
}

int main()
{
    int server_fd;
    struct sockaddr_in address;

    // Creating new socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(1);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Link and listen
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(1);
    }
    if (listen(server_fd, BACKLOG) < 0)
    {
        perror("listen");
        exit(1);
    }
    std::cout << "Server listening on port " << PORT << std::endl;

    reactor.addFd(server_fd, handle_new_connection);
    reactor.startReactor();
    return 0;
}