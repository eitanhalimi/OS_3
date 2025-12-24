// q8/proactor.cpp
#include "proactor.hpp"

#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <cerrno>
#include <cstring>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

struct Proactor {
    int sockfd;
    proactorFunc callback;

    std::thread accept_thread;
    std::atomic<bool> running{false};

    std::mutex mtx;
    std::vector<std::thread> client_threads;
};

static void accept_loop(Proactor* p)
{
    p->running.store(true);

    while (p->running.load()) {
        int client_fd = ::accept(p->sockfd, nullptr, nullptr);
        if (client_fd < 0) {
            // If we are stopping, accept may fail due to shutdown().
            if (!p->running.load()) {
                break;
            }

            if (errno == EINTR) {
                continue;
            }

            // For transient errors, just keep looping.
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }

            // If accept fails for any other reason, keep the proactor alive.
            // The server can decide to stop it explicitly.
            continue;
        }

        // Start a new thread for this client and keep it so we can join on stop().
        std::thread t([p, client_fd]() {
            try {
                if (p->callback) {
                    p->callback(client_fd);
                }
            } catch (...) {
                // Swallow exceptions to avoid terminating the entire process.
            }

            // Best-effort close: in our servers the handler usually closes,
            // but closing here makes the library safer and avoids FD leaks.
            ::close(client_fd);
        });

        {
            std::lock_guard<std::mutex> lock(p->mtx);
            p->client_threads.push_back(std::move(t));
        }
    }
}

pthread_t startProactor(int sockfd, proactorFunc threadFunc)
{
    if (sockfd < 0 || threadFunc == nullptr) {
        return 0;
    }

    auto* p = new Proactor();
    p->sockfd = sockfd;
    p->callback = threadFunc;

    p->accept_thread = std::thread(accept_loop, p);
    return pthread_t(p);
}

int stopProactor(void* proactor)
{
    if (!proactor) return -1;

    auto* p = static_cast<Proactor*>(proactor);

    // Signal accept loop to stop.
    p->running.store(false);

    // Unblock accept(): shutdown is enough; we do not close the listening socket here
    // because the server owns it.
    ::shutdown(p->sockfd, SHUT_RDWR);

    if (p->accept_thread.joinable()) {
        p->accept_thread.join();
    }

    // Join all client threads that were created.
    {
        std::lock_guard<std::mutex> lock(p->mtx);
        for (std::thread& t : p->client_threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        p->client_threads.clear();
    }

    delete p;
    return 0;
}
