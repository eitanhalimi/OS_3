// q10/server.cpp
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <netinet/in.h>
#include <unistd.h>
#include "..//q3/graph.hpp"
#include "..//q8/proactor.hpp"
#include <mutex>
#include <thread>
#include <condition_variable>

#define PORT 9034        // the port client will be connect to
#define BACKLOG 10       // how many pending connections queue will hold
#define MAXDATASIZE 1024 // max number of bytes we can get at once

// globals
std::list<Point> points;
std::mutex graph_mutex;
bool ch_above_100 = false;
std::condition_variable cond;
std::mutex cond_mutex;

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

void watcher_thread()
{
    std::unique_lock<std::mutex> lock(cond_mutex);
    while (true)
    {
        cond.wait(lock); // wait for notification

        if (ch_above_100)
            std::cout << "At Least 100 units belongs to CH\n";
        else
            std::cout << "At Least 100 units no longer belongs to CH\n";
    }
}

void client_thread(int client_fd)
{
    // for debug
    std::cout << "Client connected. Thread ID: " << std::this_thread::get_id() << std::endl;
    std::string reply = "Connected to " + std::to_string(PORT) + "\n";
    send(client_fd, reply.c_str(), reply.length(), 0);

    char buffer[MAXDATASIZE];

    while (true)

    {
        ssize_t valread = recv(client_fd, buffer, MAXDATASIZE - 1, 0);
        if (valread <= 0)
        {
            close(client_fd);
            std::cout << "Client disconnected" << std::endl;
            return;
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
                    ptline.erase(std::remove_if(ptline.begin(), ptline.end(), [](char c)
                                                { return c == '\n' || c == '\r'; }), // handle linux & windows new lines

                                 ptline.end());
                    newPts.push_back(stringToPoint(ptline));
                }

                std::lock_guard<std::mutex> lock(graph_mutex);
                newGraph(points, newPts);

                std::string reply = "Graph updated\n";
                send(client_fd, reply.c_str(), reply.length(), 0);
            }
            else if (cmd == "newpoint")
            {
                std::string coords;
                liness >> coords;

                std::lock_guard<std::mutex> lock(graph_mutex);
                newPoint(points, stringToPoint(coords));

                std::string reply = "Point added\n";
                send(client_fd, reply.c_str(), reply.length(), 0);
            }
            else if (cmd == "removepoint")
            {
                std::string coords;
                liness >> coords;

                std::lock_guard<std::mutex> lock(graph_mutex);
                removePoint(points, stringToPoint(coords));

                std::string reply = "Point removed\n";
                send(client_fd, reply.c_str(), reply.length(), 0);
            }
            else if (cmd == "ch")
            {
                std::lock_guard<std::mutex> lock(graph_mutex);
                float area = calcCH(points);

                bool prev = ch_above_100;
                ch_above_100 = (area >= 100);
                if (ch_above_100 != prev)
                {
                    cond.notify_one();
                }

                std::ostringstream oss;
                oss << area << "\n";
                send(client_fd, oss.str().c_str(), oss.str().length(), 0);
            }
            else if (cmd == "quit")
            {
                close(client_fd);
                std::cout << "Client disconnected" << std::endl;
                return;
            }

            // for debug
            else if (cmd == "getgraph")
            {
                std::lock_guard<std::mutex> lock(graph_mutex);
                std::string sGraph = getGraph(points);

                send(client_fd, sGraph.c_str(), sGraph.length(), 0);
            }
        }
    }
}

int main()
{
    // whather that notify if CH crossed 100
    std::thread(watcher_thread).detach();

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

    Proactor::Proactor proactor(server_fd, client_thread);
    proactor.startProactor();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
    return 0;
}
