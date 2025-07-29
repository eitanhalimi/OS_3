// // q5/reactor.hpp
#pragma once

#include <functional>
#include <map>

class Reactor
{
public:
    using Handler = std::function<void(int)>;

    Reactor();
    ~Reactor();

    void startReactor();        // starts reactor
    void stopReactor();         // stops reactor
    int addFd(int fd, Handler); // adds fd to reactor
    int removeFd(int fd);       // removes fd from reactor

private:
    void loop();
    bool running = false;
    std::map<int, Handler> handlers;
};