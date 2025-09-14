
#pragma once
#include <mutex>
#include <fstream>
#include <string>

// Author: Prabhav Rao
// I want logs that are safe across threads. I keep this simple: a single mutex around a file stream.
class Logger {
public:
    explicit Logger(const std::string& path);
    ~Logger();

    void info(const std::string& msg);
    void error(const std::string& msg);

private:
    std::mutex mtx_;
    std::ofstream out_;
};
