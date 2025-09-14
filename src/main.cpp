
#include "server.hpp"
#include <iostream>
#include <cstdlib>

// Author: Prabhav Rao
// I keep main small so the concurrency design is obvious: construct the server with a thread pool and run it.
int main(int argc, char** argv) {
    int port = 8080;
    size_t nthreads = std::thread::hardware_concurrency();
    if (argc >= 2) port = std::atoi(argv[1]);
    if (argc >= 3) nthreads = std::max(1, std::atoi(argv[2]));

    try {
        Server s("0.0.0.0", port, nthreads);
        std::cout << "Server running on port " << port << " with " << nthreads << " threads. Press Ctrl+C to stop." << std::endl;
        s.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
