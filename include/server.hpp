
#pragma once
#include <string>
#include <atomic>
#include "thread_pool.hpp"
#include "logger.hpp"

// Author: Prabhav Rao
// I wrote this server class to demonstrate real multithreading: one accept loop pushes sockets into a thread pool.
// Each worker parses HTTP and writes a response. I keep the features small so I can focus on correctness and throughput.
class Server {
public:
    Server(const std::string& host, int port, size_t nthreads);
    ~Server();

    // Start the blocking accept loop. I call this from main.
    void run();

    // Ask the server to stop. The accept loop will exit.
    void stop();

private:
    int create_listen_socket();
    void handle_client(int client_fd);

    std::string host_;
    int port_;
    int listen_fd_{-1};
    std::atomic<bool> stopping_{false};
    ThreadPool pool_;
    Logger logger_;
};
