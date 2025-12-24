// q5/reactor.cpp
#include "reactor.hpp"

#include <sys/select.h>
#include <unistd.h>

#include <errno.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

struct Reactor {
    std::unordered_map<int, reactorFunc> callbacks;
    std::mutex mtx;

    std::thread loop_thread;
    std::atomic<bool> running{false};
};

static void reactor_loop(Reactor* r)
{
    r->running.store(true);

    while (r->running.load()) {
        fd_set readfds;
        FD_ZERO(&readfds);

        int maxfd = -1;

        {
            std::lock_guard<std::mutex> lock(r->mtx);
            for (const auto& kv : r->callbacks) {
                int fd = kv.first;
                if (fd >= 0) {
                    FD_SET(fd, &readfds);
                    if (fd > maxfd) maxfd = fd;
                }
            }
        }

        // If no fds are registered, avoid busy-loop
        if (maxfd < 0) {
            usleep(1000); // 1ms
            continue;
        }

        // Optional timeout to allow clean stop without being stuck forever
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 200000; // 200ms

        int ready = select(maxfd + 1, &readfds, nullptr, nullptr, &tv);
        if (ready < 0) {
            if (errno == EINTR) continue;
            // Serious error: stop the loop
            break;
        }
        if (ready == 0) {
            // timeout
            continue;
        }

        // Collect ready callbacks without holding lock during callbacks
        std::vector<std::pair<int, reactorFunc>> to_call;
        {
            std::lock_guard<std::mutex> lock(r->mtx);
            to_call.reserve(r->callbacks.size());
            for (const auto& kv : r->callbacks) {
                int fd = kv.first;
                if (FD_ISSET(fd, &readfds)) {
                    to_call.push_back(kv);
                }
            }
        }

        for (const auto& item : to_call) {
            int fd = item.first;
            reactorFunc fn = item.second;
            if (fn) {
                fn(fd);
            }
        }
    }

    r->running.store(false);
}

void* startReactor()
{
    auto* r = new Reactor();
    r->loop_thread = std::thread(reactor_loop, r);
    return static_cast<void*>(r);
}

int addFdToReactor(void* reactor, int fd, reactorFunc func)
{
    if (!reactor || fd < 0 || !func) return -1;

    auto* r = static_cast<Reactor*>(reactor);
    std::lock_guard<std::mutex> lock(r->mtx);

    r->callbacks[fd] = func;
    return 0;
}

int removeFdFromReactor(void* reactor, int fd)
{
    if (!reactor || fd < 0) return -1;

    auto* r = static_cast<Reactor*>(reactor);
    std::lock_guard<std::mutex> lock(r->mtx);

    auto it = r->callbacks.find(fd);
    if (it == r->callbacks.end()) return -1;

    r->callbacks.erase(it);
    return 0;
}

int stopReactor(void* reactor)
{
    if (!reactor) return -1;

    auto* r = static_cast<Reactor*>(reactor);

    // Signal loop to stop
    r->running.store(false);

    if (r->loop_thread.joinable()) {
        r->loop_thread.join();
    }

    delete r;
    return 0;
}
