// q8/proactor.hpp
#pragma once
#include <thread>
#include <functional>
#include <atomic>

namespace Proactor
{

    // Type of function that handles each client: takes int sockfd
    using proactorFunc = std::function<void(int)>;

    // Starts the proactor: listens on given socket and spawns threads for clients
    class Proactor
    {
    private:
        void accept_loop();

        int sockfd;
        proactorFunc client_handler;
        std::thread acceptor_thread;
        std::atomic<bool> running = false;

    public:
        Proactor(int listening_fd, proactorFunc threadFunc);
        ~Proactor();

        // Starts the accept loop in background
        void startProactor();

        // Stops the proactor loop
        void stopProactor();
    };

}
