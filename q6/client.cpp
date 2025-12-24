// client.cpp
// Generic interactive TCP client (line-based), auto-connects to localhost:9034.
// Works well with your q4 server protocol: sends commands and prints server prompts/responses.

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>

static constexpr const char* SERVER_IP = "127.0.0.1";
static constexpr int SERVER_PORT = 9034;
static constexpr int BUF_SIZE = 4096;

static void die(const std::string& msg)
{
    std::cerr << msg << " (errno=" << errno << "): " << std::strerror(errno) << "\n";
    std::exit(1);
}

int main()
{
    // 1) Create socket
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        die("socket() failed");

    // 2) Build address
    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(SERVER_PORT);

    if (::inet_pton(AF_INET, SERVER_IP, &serv.sin_addr) != 1)
    {
        std::cerr << "inet_pton() failed for IP: " << SERVER_IP << "\n";
        ::close(sockfd);
        return 1;
    }

    // 3) Connect
    if (::connect(sockfd, reinterpret_cast<sockaddr*>(&serv), sizeof(serv)) < 0)
        die("connect() failed");

    std::cout << "Connected to server " << SERVER_IP << ":" << SERVER_PORT << "\n";
    std::cout << "Enter commands (e.g., newgraph, newpoint, removepoint, ch, getgraph, quit...)\n";
    std::cout << "> " << std::flush;

    // 4) Event loop: read from BOTH stdin and socket using select()
    bool running = true;
    char buf[BUF_SIZE];

    while (running)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int maxfd = (sockfd > STDIN_FILENO) ? sockfd : STDIN_FILENO;

        int rc = ::select(maxfd + 1, &readfds, nullptr, nullptr, nullptr);
        if (rc < 0)
        {
            if (errno == EINTR)
                continue; // interrupted by signal, retry
            die("select() failed");
        }

        // If server sent data
        if (FD_ISSET(sockfd, &readfds))
        {
            ssize_t n = ::recv(sockfd, buf, BUF_SIZE - 1, 0);
            if (n == 0)
            {
                std::cout << "\nServer closed the connection.\n";
                break;
            }
            if (n < 0)
                die("recv() failed");

            buf[n] = '\0';
            std::cout << "\n" << buf;
            std::cout << "> " << std::flush;
        }

        // If user typed something
        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            std::string line;
            if (!std::getline(std::cin, line))
            {
                // stdin closed (Ctrl+D)
                std::cout << "\nInput closed. Sending 'quit' and exiting...\n";
                std::string q = "quit\n";
                ::send(sockfd, q.c_str(), q.size(), 0);
                break;
            }

            // Always send with newline (server expects line-based commands)
            line.push_back('\n');

            ssize_t sent = ::send(sockfd, line.c_str(), line.size(), 0);
            if (sent < 0)
                die("send() failed");

            // Optional: if user typed quit, exit gracefully (server will close too)
            // We check the original line without the '\n'
            if (line == "quit\n" || line == "QUIT\n" || line == "Quit\n")
            {
                running = false;
                // We'll break after we give the server a chance to respond/close
                // (no need to wait forever).
            }

            std::cout << "> " << std::flush;
        }
    }

    ::close(sockfd);
    std::cout << "Client exited.\n";
    return 0;
}
