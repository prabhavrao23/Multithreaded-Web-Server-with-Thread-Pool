
# Multithreaded Web Server with Thread Pool

**Author:** Prabhav Rao

I built this project to demonstrate real multithreading in C++ by serving HTTP requests concurrently with a custom thread pool. I focused on correctness, simplicity, and measurable throughput gains over a single threaded baseline.

## Features
- Thread pool that runs request handlers concurrently
- POSIX sockets accept loop and minimal HTTP parsing
- Simple routes: `/`, `/hello`, `/ping`
- Thread safe file logger
- Clean CMake build

## Build and Run

### Prerequisites
- Linux or macOS
- CMake 3.12+
- A C++17 compiler
- `ab` (ApacheBench) for quick benchmarking (optional)

### Steps
```bash
git clone <your-fork-or-path>
cd MultithreadedWebServer
mkdir -p build && cd build
cmake ..
cmake --build . -j
./webserver 8080 8   # port threads
```

Open a browser:
- http://localhost:8080/
- http://localhost:8080/ping

## Benchmarking

I used ApacheBench to compare throughput.

Single threaded:
```bash
./webserver 8080 1 &
ab -n 1000 -c 50 http://127.0.0.1:8080/ping
```

Multithreaded:
```bash
./webserver 8080 8 &
ab -n 1000 -c 50 http://127.0.0.1:8080/ping
```

On a modern laptop, I expect a 3x to 6x improvement in requests per second when moving from 1 thread to N threads, depending on CPU cores and kernel settings. You should include your own measured numbers in your resume bullet.

## Project Structure
```
MultithreadedWebServer/
├── CMakeLists.txt
├── include/
│   ├── http.hpp
│   ├── logger.hpp
│   ├── server.hpp
│   └── thread_pool.hpp
└── src/
    ├── http.cpp
    ├── logger.cpp
    ├── main.cpp
    ├── server.cpp
    └── thread_pool.cpp
```

## Design Notes

- I keep the thread pool minimal: a job queue protected by a mutex and a condition variable. Each worker loops until shutdown.
- The server uses a blocking `accept` and enqueues each client socket as a job. This highlights the concurrency bottleneck clearly when I change thread counts.
- The HTTP parser is intentionally tiny. If I extend this, I will add request bodies, persistent connections, and better routing.

## Safety and Limits

- This server closes the connection after each response. That keeps the code simple and avoids partial reads and stateful parsing.
- Error handling is minimal but enough to keep the process stable under load.

## License
MIT
