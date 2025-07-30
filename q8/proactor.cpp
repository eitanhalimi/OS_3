#include "proactor.hpp"
#include <unistd.h>
#include <iostream>
#include <atomic>
#include <mutex>
#include <sys/socket.h>

namespace Proactor {

    Server::Server(int listening_fd, ClientHandler handler)
        : sockfd(listening_fd), client_handler(std::move(handler)) {}

    Server::~Server() {
        stop(); // make sure we stop the loop on destruction
    }

    void Server::start() {
        running = true;
        acceptor_thread = std::thread(&Server::accept_loop, this);
    }

    void Server::stop() {
        if (running) {
            running = false;
            close(sockfd); // force accept() to fail and exit loop
            if (acceptor_thread.joinable()) {
                acceptor_thread.join();
            }
        }
    }

    void Server::accept_loop() {
        while (running) {
            int client_fd = accept(sockfd, nullptr, nullptr);
            if (client_fd < 0) {
                if (running) {
                    perror("[Proactor] accept failed");
                }
                continue;
            }

            // Create detached thread to handle client
            std::thread(client_handler, client_fd).detach();
        }
    }

}
