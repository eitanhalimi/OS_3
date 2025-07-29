// q5/reactor.hpp
#pragma once
#include <functional>
#include <thread>
#include <mutex>
#include <vector>
// #include <atomic>
#include <map>

class Reactor
{
using ReactorFunc = std::function<void(int)>;

private:
    void loop();
    std::map<int, ReactorFunc> handlers;
    std::recursive_mutex handlers_mutex;
    std::thread thread;
    bool running{false};

public:
    Reactor();
    ~Reactor();

    // Disable copy
    Reactor(const Reactor &) = delete;
    Reactor &operator=(const Reactor &) = delete;

    void startReactor();
    void stopReactor();

    // returns 0 on success, -1 on error
    int addFd(int fd, ReactorFunc func);
    int removeFd(int fd);
};
