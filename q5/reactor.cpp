// // q5/reactor.cpp
#include "reactor.hpp"
#include <sys/select.h>
#include <unistd.h>
#include <iostream>

Reactor::Reactor() = default;

Reactor::~Reactor()
{
    stopReactor();
}

void Reactor::startReactor()
{
    running = true;
    loop(); // runs in calling thread
}

void Reactor::stopReactor()
{
    running = false;
}

int Reactor::addFd(int fd, Handler handler)
{
    if (fd < 0)
        return -1;
    handlers[fd] = handler;
    return 0;
}

int Reactor::removeFd(int fd)
{
    return handlers.erase(fd) > 0 ? 0 : -1;
}

void Reactor::loop()
{
    while (running)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        int maxfd = -1;

        for (const auto &[fd, _] : handlers)
        {
            FD_SET(fd, &readfds);
            if (fd > maxfd)
                maxfd = fd;
        }

        if (maxfd < 0)
        {
            usleep(10000); // no active FDs
            continue;
        }

        timeval timeout = {1, 0}; // 1 second
        int ready = select(maxfd + 1, &readfds, nullptr, nullptr, &timeout);

        if (ready < 0)
        {
            perror("select");
            continue;
        }

        std::vector<int> ready_fds;
        for (const auto &[fd, _] : handlers)
        {
            if (FD_ISSET(fd, &readfds))
            {
                ready_fds.push_back(fd);
            }
        }

        for (int fd : ready_fds)
        {
            try
            {
                if (handlers.count(fd))
                {
                    handlers.at(fd)(fd);
                }
            }
            catch (...)
            {
                std::cerr << "Exception in handler\n";
            }
        }
    }
}
