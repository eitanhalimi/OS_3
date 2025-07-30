// q8/proactor.cpp
#include "proactor.hpp"
#include <unistd.h>
#include <iostream>
#include <atomic>
#include <mutex>
#include <sys/socket.h>

namespace Proactor {

    Proactor::Proactor(int listening_fd, proactorFunc threadFunc)
        : sockfd(listening_fd), client_handler(std::move(threadFunc)) {}

    Proactor::~Proactor() {
        stopProactor(); // make sure we stop the loop on destruction
    }

    void Proactor::startProactor() {
        running = true;
        acceptor_thread = std::thread(&Proactor::accept_loop, this);
    }

    void Proactor::stopProactor() {
        if (running) {
            running = false;
            close(sockfd); // force accept() to fail and exit loop
            if (acceptor_thread.joinable()) {
                acceptor_thread.join();
            }
        }
    }

    void Proactor::accept_loop() {
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
