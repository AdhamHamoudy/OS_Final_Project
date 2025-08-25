#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "graph.hpp"
#include "../Stage6/server_protocol.hpp"
#include "algorithms.hpp"        // GraphAlgorithm + factory

// Receive one line from socket
static bool recv_line(int fd, std::string& line) {
    line.clear();
    char c;
    while (true) {
        ssize_t r = ::recv(fd, &c, 1, 0);
        if (r <= 0) return false;
        if (c == '\n') break;
        line.push_back(c);
    }
    if (!line.empty() && line.back() == '\r') line.pop_back();
    return true;
}

// Send string to client
static bool send_str(int fd, const std::string& s) {
    const char* p = s.c_str(); size_t left = s.size();
    while (left) {
        ssize_t w = ::send(fd, p, left, 0);
        if (w <= 0) return false;
        p += w;
        left -= (size_t)w;
    }
    return true;
}

// Handle one client request
static bool handle_client(int fd) {
    std::string line;
    if (!recv_line(fd, line)) return false;

    // Expected: ALG <ALGONAME> RAND ...   or   ALG <ALGONAME> FILE ...
    auto toks = split_ws(line);
    if (toks.size() < 2 || toks[0] != "ALG") {
        send_str(fd, "ERR bad request\nEND\n");
        return true;
    }

    std::string alg_name = toks[1];
    Graph G(0);

    if (toks[2] == "RAND") {
        if (toks.size() != 6) {
            send_str(fd, "ERR RAND usage\nEND\n");
            return true;
        }
        size_t n = std::stoul(toks[3]);
        size_t m = std::stoul(toks[4]);
        unsigned seed = (unsigned)std::stoul(toks[5]);
        try {
            G = Graph::random_simple(n, m, seed);
        } catch (const std::exception& e) {
            send_str(fd, std::string("ERR ") + e.what() + "\nEND\n");
            return true;
        }
    } else if (toks[2] == "FILE") {
        // Read n m
        if (!recv_line(fd, line)) {
            send_str(fd, "ERR missing n m\nEND\n");
            return true;
        }
        auto nm = split_ws(line);
        if (nm.size() != 2) {
            send_str(fd, "ERR bad n m\nEND\n");
            return true;
        }
        size_t n = std::stoul(nm[0]);
        size_t m = std::stoul(nm[1]);
        G = Graph(n);

        // Read m edges
        for (size_t i = 0; i < m; ++i) {
            if (!recv_line(fd, line)) {
                send_str(fd, "ERR missing edges\nEND\n");
                return true;
            }
            auto uv = split_ws(line);
            if (uv.size() != 2) {
                send_str(fd, "ERR bad edge\nEND\n");
                return true;
            }
            size_t u = std::stoul(uv[0]);
            size_t v = std::stoul(uv[1]);
            if (!G.add_edge(u, v)) {
                send_str(fd, "ERR invalid/duplicate edge\nEND\n");
                return true;
            }
        }

        // Expect END
        if (!recv_line(fd, line) || line != "END") {
            send_str(fd, "ERR expected END\nEND\n");
            return true;
        }
    } else {
        send_str(fd, "ERR unknown input mode\nEND\n");
        return true;
    }

    // Create algorithm using Factory
    GraphAlgorithm* alg = create_algorithm(alg_name);
    if (!alg) {
        send_str(fd, "ERR unknown algorithm\nEND\n");
        return true;
    }

    // Run algorithm
    std::string result = alg->run(G);
    delete alg;  // avoid leaks

    // If algorithm already prefixed with ERR, send as-is
    if (result.rfind("ERR", 0) == 0) {
        send_str(fd, result + "\nEND\n");
    } else {
        send_str(fd, result + "\nEND\n");
    }
    return true;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>\n";
        return 1;
    }
    int port = std::stoi(argv[1]);

    int s = ::socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { perror("socket"); return 2; }
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((uint16_t)port);
    if (::bind(s, (sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); return 3; }
    if (::listen(s, 64) < 0) { perror("listen"); return 4; }

    std::cout << "Algorithm server listening on port " << port << "...\n";
    while (true) {
        int c = ::accept(s, nullptr, nullptr);
        if (c < 0) { perror("accept"); continue; }
        handle_client(c);
        ::close(c);
    }
}
