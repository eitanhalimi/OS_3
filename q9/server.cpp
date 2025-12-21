// q9/server.cpp
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <mutex>
#include <thread>
#include <chrono>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "../q3/graph.hpp"
#include "../q8/proactor.hpp"

#define PORT 9034        // the port client will be connect to
#define BACKLOG 10       // how many pending connections queue will hold
#define MAXDATASIZE 1024 // max number of bytes we can get at once

// globals
std::list<Point> points;
std::mutex graph_mutex;

// helper, for debug
static std::string getGraph(const std::list<Point>& graph)
{
    std::string s = "the graph is:\n";
    for (const Point& p : graph)
    {
        s += "(" + std::to_string(p.x) + "," + std::to_string(p.y) + ")\n";
    }
    return s;
}

static void* client_thread(int client_fd)
{
    // for debug
    std::cout << "Client connected. Thread ID: " << std::this_thread::get_id() << std::endl;

    std::string reply = "Connected to " + std::to_string(PORT) + "\n";
    ::send(client_fd, reply.c_str(), reply.length(), 0);

    char buffer[MAXDATASIZE];

    while (true)
    {
        ssize_t valread = ::recv(client_fd, buffer, MAXDATASIZE - 1, 0);
        if (valread <= 0)
        {
            std::cout << "Client disconnected" << std::endl;
            return nullptr;
        }

        buffer[valread] = '\0';
        std::istringstream iss(buffer);
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
                newPts.reserve((n > 0) ? static_cast<size_t>(n) : 0);

                // get n points from client
                for (int i = 0; i < n; ++i)
                {
                    ::send(client_fd, "Enter point X,Y:\n", 17, 0);

                    ssize_t read = ::recv(client_fd, buffer, MAXDATASIZE - 1, 0);
                    if (read <= 0)
                    {
                        // client disconnected mid-input
                        std::cout << "Client disconnected" << std::endl;
                        return nullptr;
                    }

                    buffer[read] = '\0';
                    std::string ptline = buffer;

                    // handle linux & windows new lines
                    ptline.erase(std::remove_if(ptline.begin(), ptline.end(),
                                                [](char c) { return c == '\n' || c == '\r'; }),
                                 ptline.end());

                    newPts.push_back(stringToPoint(ptline));
                }

                {
                    std::lock_guard<std::mutex> lock(graph_mutex);
                    newGraph(points, newPts);
                }

                const std::string reply = "Graph updated\n";
                ::send(client_fd, reply.c_str(), reply.length(), 0);
            }
            else if (cmd == "newpoint")
            {
                std::string coords;
                liness >> coords;

                {
                    std::lock_guard<std::mutex> lock(graph_mutex);
                    newPoint(points, stringToPoint(coords));
                }

                const std::string reply = "Point added\n";
                ::send(client_fd, reply.c_str(), reply.length(), 0);
            }
            else if (cmd == "removepoint")
            {
                std::string coords;
                liness >> coords;

                {
                    std::lock_guard<std::mutex> lock(graph_mutex);
                    removePoint(points, stringToPoint(coords));
                }

                const std::string reply = "Point removed\n";
                ::send(client_fd, reply.c_str(), reply.length(), 0);
            }
            else if (cmd == "ch")
            {
                float area = 0.0f;
                {
                    std::lock_guard<std::mutex> lock(graph_mutex);
                    area = calcCH(points);
                }

                std::ostringstream oss;
                oss << area << "\n";
                ::send(client_fd, oss.str().c_str(), oss.str().length(), 0);
            }
            else if (cmd == "quit")
            {
                std::cout << "Client disconnected" << std::endl;
                return nullptr;
            }
            // for debug
            else if (cmd == "getgraph")
            {
                std::string sGraph;
                {
                    std::lock_guard<std::mutex> lock(graph_mutex);
                    sGraph = getGraph(points);
                }
                ::send(client_fd, sGraph.c_str(), sGraph.length(), 0);
            }
            else
            {
                const std::string reply = "Unknown command\n";
                ::send(client_fd, reply.c_str(), reply.length(), 0);
            }
        }
    }
}

int main()
{
    int server_fd;
    struct sockaddr_in address{};

    // Creating new socket
    server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket failed");
        return 1;
    }

    int opt = 1;
    ::setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind and listen
    if (::bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        ::close(server_fd);
        return 1;
    }
    if (::listen(server_fd, BACKLOG) < 0)
    {
        perror("listen");
        ::close(server_fd);
        return 1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    pthread_t p = startProactor(server_fd, client_thread);
    if (!p)
    {
        std::cerr << "Failed to start proactor\n";
        ::close(server_fd);
        return 1;
    }

    // Keep process alive
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }

    // Unreachable in normal flow, but keep it correct.
    stopProactor(p);
    ::close(server_fd);
    return 0;
}
