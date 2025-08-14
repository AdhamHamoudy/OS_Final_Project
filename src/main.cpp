#include "graph.hpp"
#include "euler.hpp"
#include <getopt.h>
#include <iostream>
#include <optional>
#include <string>

static void usage(const char* prog) {
    std::cerr
        << "Usage: " << prog << " [-i input_file] [-n vertices -m edges -s seed]\n"
        << "  -i <path>    Load graph from file (format: n m then m lines of 'u v')\n"
        << "  -n <int>     Number of vertices for random graph\n"
        << "  -m <int>     Number of edges for random graph\n"
        << "  -s <int>     Random seed for random graph\n"
        << "Examples:\n"
        << "  " << prog << " -i graph.txt\n"
        << "  " << prog << " -n 10 -m 15 -s 123\n";
}

int main(int argc, char** argv) {
    std::string input_path;
    long n = -1, m = -1, seed = 1;

    int opt;
    while ((opt = ::getopt(argc, argv, "i:n:m:s:")) != -1) {
        switch (opt) {
            case 'i': input_path = optarg; break;
            case 'n': n = std::stol(optarg); break;
            case 'm': m = std::stol(optarg); break;
            case 's': seed = std::stol(optarg); break;
            default:
                usage(argv[0]);
                return 1;
        }
    }

    std::optional<Graph> Gopt;
    if (!input_path.empty()) {
        Gopt = Graph::load_from_file(input_path);
        if (!Gopt) {
            std::cerr << "Failed to load graph from '" << input_path << "'\n";
            return 2;
        }
    } else {
        if (n < 0 || m < 0) {
            usage(argv[0]);
            return 1;
        }
        try {
            Gopt = Graph::random_simple(static_cast<std::size_t>(n),
                                        static_cast<std::size_t>(m),
                                        static_cast<unsigned>(seed));
        } catch (const std::exception& e) {
            std::cerr << "Error generating random graph: " << e.what() << "\n";
            return 3;
        }
    }
    const Graph& G = *Gopt;

    auto check = euler_feasibility(G);
    if (!check.ok) {
        std::cout << "NO EULER CIRCUIT: " << check.reason << "\n";
        return 0;
    }

    auto circuit = find_euler_circuit(G);
    if (circuit.empty()) {
        // Shouldn't happen if feasibility passed, but guard anyway.
        std::cout << "NO EULER CIRCUIT: internal failure\n";
        return 0;
    }

    std::cout << "EULER CIRCUIT LENGTH (edges): " << circuit.size() - 1 << "\n";
    std::cout << "CIRCUIT:";
    for (auto v : circuit) std::cout << " " << v;
    std::cout << "\n";
    return 0;
}
