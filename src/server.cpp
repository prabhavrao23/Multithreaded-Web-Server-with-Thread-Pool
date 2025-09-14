
#include "server.hpp"
#include "http.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <stdexcept>
#include <sstream>

Server::Server(const std::string& host, int port, size_t nthreads)
    : host_(host), port_(port), pool_(nthreads), logger_("server.log") {
    listen_fd_ = create_listen_socket();
}

Server::~Server() {
    stop();
    if (listen_fd_ >= 0) close(listen_fd_);
}

int Server::create_listen_socket() {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) throw std::runtime_error("socket failed");

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        close(fd);
        throw std::runtime_error("bind failed");
    }
    if (listen(fd, 128) < 0) {
        close(fd);
        throw std::runtime_error("listen failed");
    }
    logger_.info("Listening on 0.0.0.0:" + std::to_string(port_));
    return fd;
}

void Server::run() {
    while (!stopping_) {
        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        int client_fd = accept(listen_fd_, (sockaddr*)&client_addr, &len);
        if (client_fd < 0) {
            if (errno == EINTR) continue;
            logger_.error("accept failed");
            continue;
        }
        pool_.enqueue([this, client_fd] { handle_client(client_fd); });
    }
    pool_.shutdown();
}

void Server::stop() {
    bool expected = false;
    if (stopping_.compare_exchange_strong(expected, true)) {
        shutdown(listen_fd_, SHUT_RDWR);
    }
}

void Server::handle_client(int client_fd) {
    // I read a small buffer and parse a minimal HTTP request.
    char buf[4096];
    ssize_t n = ::recv(client_fd, buf, sizeof(buf), 0);
    std::string raw = (n > 0) ? std::string(buf, buf + n) : std::string();

    HttpRequest req;
    if (!parse_http_request(raw, req)) {
        HttpResponse resp;
        resp.status = 400;
        resp.status_text = "Bad Request";
        resp.body = "bad request";
        auto bytes = to_http_string(resp);
        send(client_fd, bytes.data(), bytes.size(), 0);
        close(client_fd);
        return;
    }

    // Simple routing for demo:
    HttpResponse resp;
    if (req.path == "/" || req.path == "/hello") {
        resp.content_type = "text/html";
        resp.body = "<html><body><h1>Hello from Prabhav's multithreaded server</h1></body></html>";
    } else if (req.path == "/ping") {
        resp.content_type = "application/json";
        resp.body = "{\"ok\":true}";
    } else {
        resp.status = 404;
        resp.status_text = "Not Found";
        resp.body = "not found";
    }

    auto bytes = to_http_string(resp);
    send(client_fd, bytes.data(), bytes.size(), 0);
    close(client_fd);
    logger_.info("Served " + req.method + " " + req.path);
}
