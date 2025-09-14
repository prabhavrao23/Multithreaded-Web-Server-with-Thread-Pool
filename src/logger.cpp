
#include "logger.hpp"
#include <chrono>
#include <iomanip>

static std::string now_ts() {
    using namespace std::chrono;
    auto t = system_clock::to_time_t(system_clock::now());
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return buf;
}

Logger::Logger(const std::string& path) : out_(path, std::ios::app) {}
Logger::~Logger() { out_.flush(); }

void Logger::info(const std::string& msg) {
    std::lock_guard<std::mutex> lock(mtx_);
    out_ << now_ts() << " [INFO] " << msg << "\n";
    out_.flush();
}

void Logger::error(const std::string& msg) {
    std::lock_guard<std::mutex> lock(mtx_);
    out_ << now_ts() << " [ERROR] " << msg << "\n";
    out_.flush();
}
