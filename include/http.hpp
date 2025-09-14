
#pragma once
#include <string>
#include <unordered_map>
#include <sstream>

// Author: Prabhav Rao
// I keep this tiny HTTP parser minimal. I only need GET and basic headers to demonstrate concurrency.
struct HttpRequest {
    std::string method;
    std::string path;
    std::unordered_map<std::string, std::string> headers;
};

struct HttpResponse {
    int status = 200;
    std::string status_text = "OK";
    std::string content_type = "text/plain";
    std::string body = "hello";
};

// Very small helper to parse the request line and a few headers.
bool parse_http_request(const std::string& raw, HttpRequest& out);

// Serialize a response into raw HTTP bytes.
std::string to_http_string(const HttpResponse& resp);
