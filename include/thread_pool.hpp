
#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

// Author: Prabhav Rao
// I wrote this header to implement a simple thread pool so my web server can process many client sockets at once.
// I keep the interface tiny on purpose so I can reason about correctness under load.
class ThreadPool {
public:
    explicit ThreadPool(size_t nthreads);
    ~ThreadPool();

    // Enqueue a unit of work. I accept a generic callable with no args for simplicity.
    void enqueue(std::function<void()> job);

    // Ask threads to stop once the queue is empty.
    void shutdown();

private:
    void worker_loop(size_t id);

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> jobs_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::atomic<bool> stopping_{false};
};
