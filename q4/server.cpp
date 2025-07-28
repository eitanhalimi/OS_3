// q4/server.cpp
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <netinet/in.h>
#include <unistd.h>
#include "..//q3/graph.hpp"

#define PORT 9034        // the port client will be connecting to
#define BACKLOG 10       // how many pending connections queue will hold
#define MAXDATASIZE 1024 // max number of bytes we can get at once

std::list<Point> points;

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

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in address;
    char buffer[MAXDATASIZE];
    socklen_t addrlen = sizeof(address);

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

    while (true)
    {
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0)
        {
            perror("accept");
            continue;
        }
        std::cout << "Client connected" << std::endl;
        std::string reply = "Connected to " + std::to_string(PORT) + "\n";
        send(client_fd, reply.c_str(), reply.length(), 0);

        // commands from client
        ssize_t valread;
        while ((valread = recv(client_fd, buffer, MAXDATASIZE - 1, 0)) > 0)
        {
            buffer[valread] = '\0';
            std::string input = buffer;

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
                    std::vector<Point> newPts;

                    // get n points from client
                    for (int i = 0; i < n; ++i)
                    {
                        std::string ptline;
                        send(client_fd, "Enter point X,Y:\n", 17, 0);
                        ssize_t read = recv(client_fd, buffer, 1023, 0);
                        if (read <= 0)
                            break;
                        buffer[read] = '\0';
                        ptline = buffer;
                        ptline.erase(std::remove(ptline.begin(), ptline.end(), '\n'), ptline.end());
                        newPts.push_back(stringToPoint(ptline));
                    }
                    newGraph(points, newPts);
                    std::string reply = "Graph updated\n";
                    send(client_fd, reply.c_str(), reply.length(), 0);
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
                    close(client_fd);
                    std::cout << "Client disconnected" << std::endl;
                    break; // Exit the inner loop to handle next client
                }

                // for debug
                else if (cmd == "getgraph")
                {
                    std::string sGraph = getGraph(points);
                    send(client_fd, sGraph.c_str(), sGraph.length(), 0);
                }
            }
        }
        close(client_fd);
        std::cout << "Client disconnected" << std::endl;
    }
    return 0;
}
