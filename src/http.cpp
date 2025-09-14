
#include "http.hpp"
#include <algorithm>

static inline std::string trim(const std::string& s) {
    size_t b = s.find_first_not_of(" \r\n\t");
    if (b == std::string::npos) return "";
    size_t e = s.find_last_not_of(" \r\n\t");
    return s.substr(b, e - b + 1);
}

bool parse_http_request(const std::string& raw, HttpRequest& out) {
    std::istringstream ss(raw);
    std::string line;
    if (!std::getline(ss, line)) return false;
    if (!line.empty() && line.back() == '\r') line.pop_back();

    std::istringstream first(line);
    if (!(first >> out.method >> out.path)) return false;

    // Headers
    while (std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) break; // end of headers
        auto pos = line.find(':');
        if (pos == std::string::npos) continue;
        std::string key = trim(line.substr(0, pos));
        std::string val = trim(line.substr(pos + 1));
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        out.headers[key] = val;
    }
    return true;
}

std::string to_http_string(const HttpResponse& resp) {
    std::ostringstream out;
    out << "HTTP/1.1 " << resp.status << " " << resp.status_text << "\r\n";
    out << "Content-Type: " << resp.content_type << "\r\n";
    out << "Content-Length: " << resp.body.size() << "\r\n";
    out << "Connection: close\r\n";
    out << "\r\n";
    out << resp.body;
    return out.str();
}
