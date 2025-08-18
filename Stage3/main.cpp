#include "graph.hpp"
#include "euler.hpp"

#include <getopt.h>     // POSIX getopt(3)
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

static void print_usage(const char* prog) {
    std::cerr <<
        "Usage:\n"
        "  " << prog << " -f <graph_file>\n"
        "  " << prog << " -n <vertices> -m <edges> -s <seed>\n"
        "Options:\n"
        "  -f <file>   Load graph from file. First line: n m; then m lines: u v\n"
        "  -n <num>    Number of vertices (random graph mode)\n"
        "  -m <num>    Number of edges (random graph mode)\n"
        "  -s <num>    Random seed (unsigned) (random graph mode)\n"
        "  -h          Show this help\n";
}

int main(int argc, char** argv) {
    std::string file_path;
    bool have_file = false;

    std::size_t n = 0;
    std::size_t m = 0;
    unsigned seed = 0;
    bool have_n = false, have_m = false, have_s = false;

    int opt;
    while ((opt = getopt(argc, argv, "f:n:m:s:h")) != -1) {
        switch (opt) {
            case 'f':
                file_path = optarg ? std::string(optarg) : std::string();
                have_file = true;
                break;
            case 'n':
                if (!optarg) { print_usage(argv[0]); return EXIT_FAILURE; }
                n = static_cast<std::size_t>(std::strtoull(optarg, nullptr, 10));
                have_n = true;
                break;
            case 'm':
                if (!optarg) { print_usage(argv[0]); return EXIT_FAILURE; }
                m = static_cast<std::size_t>(std::strtoull(optarg, nullptr, 10));
                have_m = true;
                break;
            case 's':
                if (!optarg) { print_usage(argv[0]); return EXIT_FAILURE; }
                seed = static_cast<unsigned>(std::strtoul(optarg, nullptr, 10));
                have_s = true;
                break;
            case 'h':
            default:
                print_usage(argv[0]);
                return (opt == 'h') ? EXIT_SUCCESS : EXIT_FAILURE;
        }
    }

    std::optional<Graph> Gopt;

    if (have_file) {
        // Prefer file if both provided
        if (have_n || have_m || have_s) {
            std::cerr << "[info] -f provided; ignoring -n/-m/-s flags.\n";
        }
        Gopt = Graph::load_from_file(file_path);
        if (!Gopt) {
            std::cerr << "[error] Failed to load graph from '" << file_path << "'.\n";
            return EXIT_FAILURE;
        }
    } else {
        // Random mode requires all three
        if (!(have_n && have_m && have_s)) {
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
        try {
            Gopt = Graph::random_simple(n, m, seed);
        } catch (const std::exception& e) {
            std::cerr << "[error] " << e.what() << "\n";
            return EXIT_FAILURE;
        }
    }

    const Graph& G = *Gopt;

    // Stage 2: feasibility or proof of nonexistence
    EulerCheck chk = euler_feasibility(G);
    if (!chk.ok) {
        std::cout << "Euler circuit does NOT exist: " << chk.reason << "\n";
        return EXIT_SUCCESS; // program ran fine; graph just isn't Eulerian
    }

    // Find and print Euler circuit
    std::vector<std::size_t> circuit = find_euler_circuit(G);
    if (circuit.empty()) {
        // This can happen for graphs with no edges (convention). It's still OK.
        std::cout << "Euler circuit exists. (Graph has no edges; empty tour.)\n";
        return EXIT_SUCCESS;
    }

    std::cout << "Euler circuit exists.\n";
    std::cout << "Length: " << circuit.size() << "\nPath:";
    for (std::size_t v : circuit) std::cout << ' ' << v;
    std::cout << "\n";

    return EXIT_SUCCESS;
}
