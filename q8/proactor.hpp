#pragma once
#include <thread>
#include <functional>

namespace Proactor
{

    // Type of function that handles each client: takes int sockfd
    using ClientHandler = std::function<void(int)>;

    // Starts the proactor: listens on given socket and spawns threads for clients
    class Server
    {
    private:
        void accept_loop();

        int sockfd;
        ClientHandler client_handler;
        std::thread acceptor_thread;
        std::atomic<bool> running = false;

    public:
        Server(int listening_fd, ClientHandler handler);
        ~Server();

        // Starts the accept loop in background
        void start();

        // Stops the proactor loop
        void stop();
    };

}
