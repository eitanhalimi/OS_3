// q5/reactor.cpp
#include "reactor.hpp"
#include <sys/select.h>
#include <unistd.h>
#include <iostream>

Reactor::Reactor() {}

Reactor::~Reactor() {
    stopReactor();
}

void Reactor::startReactor() {
    running = true;
    thread = std::thread(&Reactor::loop, this);
}

void Reactor::stopReactor() {
    running = false;
    if (thread.joinable()) thread.join();
}

int Reactor::addFd(int fd, ReactorFunc func) {
    if (fd < 0) return -1;
    handlers[fd] = func;
    return 0;
}

int Reactor::removeFd(int fd) {
    if (handlers.count(fd)) {
        handlers.erase(fd);
        return 0;
    }
    return -1;
}

void Reactor::loop() {
    while (running) {
        fd_set readfds;
        FD_ZERO(&readfds);
        int maxfd = -1;

        for (const auto& [fd, func] : handlers) {
            FD_SET(fd, &readfds);
            if (fd > maxfd) maxfd = fd;
        }
        if (maxfd < 0) {
            usleep(10000); // no fds to watch, sleep a bit
            continue;
        }

        timeval tv = {1, 0}; // timeout to avoid blocking forever
        int res = select(maxfd + 1, &readfds, nullptr, nullptr, &tv);
        if (res < 0) {
            perror("select");
            continue;
        }
        if (res == 0) continue; // timeout, loop again

        std::vector<int> ready;
        for (const auto& [fd, func] : handlers) {
            if (FD_ISSET(fd, &readfds)) ready.push_back(fd);
        }
        for (int fd : ready) {
            try {
                handlers[fd](fd);
            } catch (...) {
                std::cerr << "Reactor handler exception!\n";
            }
        }
    }
}
