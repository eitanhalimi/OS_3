// q5/reactor.cpp
#include "reactor.hpp"
#include <sys/select.h>
#include <unistd.h>
#include <iostream>

Reactor::Reactor() {}

Reactor::~Reactor()
{
    stopReactor();
}

void Reactor::startReactor()
{
    running = true;
    thread = std::thread(&Reactor::loop, this);
}

void Reactor::stopReactor()
{
    running = false;
    if (thread.joinable())
        thread.join();
}

int Reactor::addFd(int fd, ReactorFunc func)
{
    if (fd < 0)
        return -1;
    std::lock_guard<std::recursive_mutex> lock(handlers_mutex);
    handlers[fd] = func;
    return 0;
}

int Reactor::removeFd(int fd)
{
    std::lock_guard<std::recursive_mutex> lock(handlers_mutex);
    if (handlers.count(fd))
    {
        handlers.erase(fd);
        return 0;
    }
    return -1;
}

void Reactor::loop()
{
    while (running)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        int maxfd = -1;

        {
            std::lock_guard<std::recursive_mutex> lock(handlers_mutex);
            for (const auto &[fd, func] : handlers)
            {
                FD_SET(fd, &readfds);
                maxfd = std::max(maxfd, fd);
            }
        }

        if (maxfd < 0)
        {
            usleep(10000); // no fds to watch, sleep a bit
            continue;
        }

        timeval tv = {1, 0}; // timeout to avoid blocking forever
        int res = select(maxfd + 1, &readfds, nullptr, nullptr, &tv);
        if (res < 0)
        {
            perror("select");
            continue;
        }
        if (res == 0)
            continue; // timeout, loop again

        // std::vector<int> ready;
        std::vector<std::pair<int, ReactorFunc>> ready_handlers;

        {
            std::lock_guard<std::recursive_mutex> lock(handlers_mutex);
            for (const auto &[fd, func] : handlers) {
                if (FD_ISSET(fd, &readfds)) {
                    ready_handlers.emplace_back(fd, func);
                }
            }
        }

        for (const auto &[fd, func] : ready_handlers) {
            try {
                func(fd);
            } catch (...) {
                std::cerr << "Reactor handler exception!\n";
            }
        }
    }
}
