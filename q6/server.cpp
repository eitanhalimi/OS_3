// q6/server.cpp
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "../q3/graph.hpp"
#include "../q5/reactor.hpp"

#define PORT 9034        // the port client will be connecting to
#define BACKLOG 10       // how many pending connections queue will hold
#define MAXDATASIZE 1024 // max number of bytes we can get at once

// Global graph (shared for all clients)
static std::list<Point> g_points;

// Reactor instance
static void* g_reactor = nullptr;

// Per-client state to support "newgraph N" followed by N lines of points
struct ClientState {
    std::string inbuf;               // accumulated data until we have full lines
    int expecting_points = 0;        // how many point-lines are still expected
    std::vector<Point> pending_pts;  // points collected for newgraph
};

static std::unordered_map<int, ClientState> g_clients;

// helper (debug)
static std::string getGraphString(const std::list<Point>& graph)
{
    std::string s = "the graph is:\n";
    for (const Point& p : graph) {
        s += "(" + std::to_string(p.x) + "," + std::to_string(p.y) + ")\n";
    }
    return s;
}

static void closeClient(int fd)
{
    removeFdFromReactor(g_reactor, fd);
    close(fd);
    g_clients.erase(fd);
    std::cout << "Client disconnected (fd=" << fd << ")\n";
}

static void sendStr(int fd, const std::string& s)
{
    if (!s.empty()) {
        (void)send(fd, s.c_str(), s.size(), 0);
    }
}

// Trim \r and spaces (simple)
static void rstrip(std::string& s)
{
    while (!s.empty() && (s.back() == '\r' || s.back() == ' ' || s.back() == '\t')) {
        s.pop_back();
    }
}

static void processLine(int fd, std::string line)
{
    rstrip(line);
    if (line.empty()) return;

    ClientState& st = g_clients[fd];

    // If we are currently collecting points for "newgraph N"
    if (st.expecting_points > 0) {
        try {
            Point p = stringToPoint(line);
            st.pending_pts.push_back(p);
            st.expecting_points--;

            if (st.expecting_points == 0) {
                newGraph(g_points, st.pending_pts);
                st.pending_pts.clear();
                sendStr(fd, "Graph updated\n");
            }
        } catch (...) {
            // bad point format
            st.pending_pts.clear();
            st.expecting_points = 0;
            sendStr(fd, "Error: invalid point format. Expected X,Y\n");
        }
        return;
    }

    // Normal command parsing
    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

    if (cmd == "newgraph") {
        int n = 0;
        iss >> n;
        if (n < 0) {
            sendStr(fd, "Error: N must be non-negative\n");
            return;
        }
        st.expecting_points = n;
        st.pending_pts.clear();

        if (n == 0) {
            std::vector<Point> empty;
            newGraph(g_points, empty);
            sendStr(fd, "Graph updated\n");
        } else {
            // No prompt here! client will just send next N lines.
            sendStr(fd, "OK: send points as next lines (X,Y)\n");
        }
        return;
    }

    if (cmd == "newpoint") {
        std::string coords;
        iss >> coords;
        if (coords.empty()) {
            sendStr(fd, "Error: usage newpoint X,Y\n");
            return;
        }
        try {
            newPoint(g_points, stringToPoint(coords));
            sendStr(fd, "Point added\n");
        } catch (...) {
            sendStr(fd, "Error: invalid point format. Expected X,Y\n");
        }
        return;
    }

    if (cmd == "removepoint") {
        std::string coords;
        iss >> coords;
        if (coords.empty()) {
            sendStr(fd, "Error: usage removepoint X,Y\n");
            return;
        }
        try {
            removePoint(g_points, stringToPoint(coords));
            sendStr(fd, "Point removed\n");
        } catch (...) {
            sendStr(fd, "Error: invalid point format. Expected X,Y\n");
        }
        return;
    }

    if (cmd == "ch") {
        float area = calcCH(g_points);
        std::ostringstream oss;
        oss << area << "\n";
        sendStr(fd, oss.str());
        return;
    }

    if (cmd == "getgraph") {
        sendStr(fd, getGraphString(g_points));
        return;
    }

    if (cmd == "quit") {
        closeClient(fd);
        return;
    }

    sendStr(fd, "Error: unknown command\n");
}

// reactor callback for client fd
static void* handle_client_input(int client_fd)
{
    char buffer[MAXDATASIZE];
    ssize_t n = recv(client_fd, buffer, MAXDATASIZE - 1, 0);

    if (n <= 0) {
        closeClient(client_fd);
        return nullptr;
    }

    buffer[n] = '\0';

    ClientState& st = g_clients[client_fd];
    st.inbuf += buffer;

    // Process full lines
    while (true) {
        size_t pos = st.inbuf.find('\n');
        if (pos == std::string::npos) break;

        std::string line = st.inbuf.substr(0, pos);
        st.inbuf.erase(0, pos + 1);

        processLine(client_fd, line);

        // Client might have been closed inside processLine (quit)
        if (g_clients.find(client_fd) == g_clients.end()) {
            break;
        }
    }

    return nullptr;
}

// reactor callback for server fd: accept new connections
static void* handle_accept(int server_fd)
{
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    int client_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    if (client_fd < 0) {
        return nullptr;
    }

    std::cout << "Client connected (fd=" << client_fd << ")\n";

    // init state
    g_clients[client_fd] = ClientState{};

    // hello message
    std::string reply = "Connected to " + std::to_string(PORT) + "\n";
    sendStr(client_fd, reply);

    // register client fd in reactor
    addFdToReactor(g_reactor, client_fd, handle_client_input);
    return nullptr;
}

int main()
{
    int server_fd;
    struct sockaddr_in address;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind + listen
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }
    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    std::cout << "Stage 6 server (Reactor) listening on port " << PORT << "\n";

    // Start reactor and register server fd for accept
    g_reactor = startReactor();
    addFdToReactor(g_reactor, server_fd, handle_accept);

    // Keep main thread alive
    while (true) {
        sleep(1);
    }

    // (not reached normally)
    stopReactor(g_reactor);
    close(server_fd);
    return 0;
}
