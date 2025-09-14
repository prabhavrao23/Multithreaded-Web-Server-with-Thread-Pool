
#include "thread_pool.hpp"
#include <iostream>

ThreadPool::ThreadPool(size_t nthreads) {
    if (nthreads == 0) nthreads = std::thread::hardware_concurrency();
    workers_.reserve(nthreads);
    for (size_t i = 0; i < nthreads; ++i) {
        workers_.emplace_back([this, i] { worker_loop(i); });
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
    for (auto &t : workers_) if (t.joinable()) t.join();
}

void ThreadPool::enqueue(std::function<void()> job) {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        jobs_.push(std::move(job));
    }
    cv_.notify_one();
}

void ThreadPool::shutdown() {
    bool expected = false;
    if (stopping_.compare_exchange_strong(expected, true)) {
        cv_.notify_all();
    }
}

void ThreadPool::worker_loop(size_t /*id*/) {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [&]{ return stopping_ || !jobs_.empty(); });
            if (stopping_ && jobs_.empty()) break;
            job = std::move(jobs_.front());
            jobs_.pop();
        }
        try {
            job();
        } catch (const std::exception& e) {
            std::cerr << "[ThreadPool] Job threw: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "[ThreadPool] Job threw unknown exception" << std::endl;
        }
    }
}
