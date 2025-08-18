#include "graph.hpp"

#include <algorithm>
#include <cstdint>
#include <cctype>
#include <fstream>
#include <optional>
#include <queue>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

// ---------- Construction ----------
Graph::Graph(std::size_t n)
    : m_n(n), m_m(0), adj(n) {}

// ---------- Edge updates ----------
bool Graph::add_edge(std::size_t u, std::size_t v) {
    if (u >= m_n || v >= m_n) return false; // out of range
    if (u == v) return false;               // no self-loops

    // prevent multi-edges
    auto &lu = adj[u];
    if (std::find(lu.begin(), lu.end(), v) != lu.end()) {
        return false; // already exists
    }

    adj[u].push_back(v);
    adj[v].push_back(u);
    ++m_m;
    return true;
}

// ---------- Euler helpers ----------
bool Graph::is_connected_ignoring_isolated() const {
    // Find a starting vertex with non-zero degree and count non-isolated vertices
    std::size_t start = m_n;
    std::size_t non_isolated = 0;
    for (std::size_t i = 0; i < m_n; ++i) {
        if (!adj[i].empty()) {
            ++non_isolated;
            if (start == m_n) start = i;
        }
    }

    // Trivial cases: no or one non-isolated vertex -> connected subgraph
    if (non_isolated <= 1) return true;

    // BFS from the first non-isolated vertex
    std::vector<char> vis(m_n, 0);
    std::queue<std::size_t> q;
    vis[start] = 1;
    q.push(start);
    std::size_t reached = 1; // we've reached 'start' (which is non-isolated)

    while (!q.empty()) {
        auto u = q.front();
        q.pop();
        for (std::size_t v : adj[u]) {
            if (!vis[v]) {
                vis[v] = 1;
                q.push(v);
                if (!adj[v].empty()) {
                    ++reached; // count only non-isolated vertices
                }
            }
        }
    }

    return reached == non_isolated;
}

bool Graph::all_even_degrees() const {
    for (std::size_t i = 0; i < m_n; ++i) {
        if ((adj[i].size() & 1U) != 0U) return false;
    }
    return true;
}

// ---------- I/O & generators ----------
std::optional<Graph> Graph::load_from_file(const std::string& path) {
    std::ifstream in(path);
    if (!in) return std::nullopt;

    std::string line;

    // Read first non-empty, non-comment line as "n m"
    std::size_t n = 0, m = 0;
    while (std::getline(in, line)) {
        // skip blanks and comments
        bool only_ws = true;
        for (char c : line) {
            if (!std::isspace(static_cast<unsigned char>(c))) { only_ws = false; break; }
        }
        if (only_ws || (!line.empty() && line[0] == '#')) continue;

        std::istringstream hdr(line);
        if (!(hdr >> n >> m)) return std::nullopt;
        break;
    }

    // Basic validation with 64-bit arithmetic
    const std::uint64_t nn = static_cast<std::uint64_t>(n);
    const std::uint64_t max_m = (nn * (nn - 1)) / 2ull;
    if (n == 0) {
        if (m != 0) return std::nullopt;
    } else {
        if (static_cast<std::uint64_t>(m) > max_m) return std::nullopt;
    }

    Graph g(n);

    // Read exactly m edge lines (u v). Allow skipping blank/comment lines.
    std::size_t added = 0;
    while (added < m && std::getline(in, line)) {
        // skip blanks and comments
        bool only_ws = true;
        for (char c : line) {
            if (!std::isspace(static_cast<unsigned char>(c))) { only_ws = false; break; }
        }
        if (only_ws || (!line.empty() && line[0] == '#')) continue;

        std::istringstream es(line);
        std::size_t u, v;
        if (!(es >> u >> v)) return std::nullopt;
        if (!g.add_edge(u, v)) return std::nullopt; // invalid id, self-loop, or duplicate
        ++added;
    }

    if (added != m) return std::nullopt; // file ended early

    return g;
}

Graph Graph::random_simple(std::size_t n, std::size_t m, unsigned seed) {
    // Validate parameters
    const std::uint64_t nn = static_cast<std::uint64_t>(n);
    const std::uint64_t max_m = (nn * (nn - 1)) / 2ull;
    if (m > max_m) {
        throw std::invalid_argument("m exceeds the maximum number of edges for a simple undirected graph");
    }
    if (n == 0 && m != 0) {
        throw std::invalid_argument("cannot place edges on an empty graph");
    }

    Graph g(n);
    if (m == 0 || n <= 1) return g;

    std::mt19937 rng(seed);

    // Hybrid strategy:
    //  - Sparse case (m <= max_m/2): rejection with a hash set of unique (u<v) pairs.
    //  - Dense case  (m >  max_m/2): generate all pairs (u<v), shuffle, take first m.
    if (static_cast<std::uint64_t>(m) <= max_m / 2ull) {
        // --- Sparse: rejection sampling with uniqueness set ---
        std::uniform_int_distribution<std::size_t> dist(0, n - 1);

        std::unordered_set<std::uint64_t> seen;
        seen.reserve(m * 2 + 16);

        auto pack = [](std::size_t a, std::size_t b) -> std::uint64_t {
            if (b < a) std::swap(a, b); // canonical u<v
            return (static_cast<std::uint64_t>(a) << 32) | static_cast<std::uint64_t>(b);
        };

        while (seen.size() < m) {
            std::size_t u = dist(rng);
            std::size_t v = dist(rng);
            if (u == v) continue; // no self-loops
            (void)seen.insert(pack(u, v));
        }

        for (auto key : seen) {
            std::size_t u = static_cast<std::size_t>(key >> 32);
            std::size_t v = static_cast<std::size_t>(key & 0xffffffffULL);
            g.add_edge(u, v); // guaranteed valid & unique
        }
    } else {
        // --- Dense: generate all pairs and shuffle ---
        std::vector<std::pair<std::size_t, std::size_t>> edges;
        edges.reserve(static_cast<std::size_t>(max_m));

        for (std::size_t u = 0; u < n; ++u) {
            for (std::size_t v = u + 1; v < n; ++v) {
                edges.emplace_back(u, v);
            }
        }

        std::shuffle(edges.begin(), edges.end(), rng);

        for (std::size_t i = 0; i < m; ++i) {
            auto [u, v] = edges[i];
            g.add_edge(u, v); // cannot fail: in-range & unique
        }
    }

    return g;
}
