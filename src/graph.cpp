#include "graph.hpp"
#include <fstream>
#include <queue>
#include <random>
#include <unordered_set>
#include <algorithm>
#include <stdexcept>

Graph::Graph(std::size_t n) : m_n(n), m_m(0), adj(n) {}

bool Graph::add_edge(std::size_t u, std::size_t v) {
    if (u >= m_n || v >= m_n) return false;
    if (u == v) return false; // no self-loops
    // prevent multi-edge: check adjacency lists (keep small; O(deg))
    auto& Au = adj[u];
    if (std::find(Au.begin(), Au.end(), v) != Au.end()) return false;
    auto& Av = adj[v];
    if (std::find(Av.begin(), Av.end(), u) != Av.end()) return false;
    Au.push_back(v);
    Av.push_back(u);
    ++m_m;
    return true;
}

std::optional<Graph> Graph::load_from_file(const std::string& path) {
    std::ifstream in(path);
    if (!in) return std::nullopt;
    std::size_t n, m;
    if (!(in >> n >> m)) return std::nullopt;
    Graph G(n);
    for (std::size_t i = 0; i < m; ++i) {
        std::size_t u, v;
        if (!(in >> u >> v)) return std::nullopt;
        if (!G.add_edge(u, v)) return std::nullopt;
    }
    return G;
}

// Generate a simple undirected graph with m distinct edges chosen uniformly at random.
Graph Graph::random_simple(std::size_t n, std::size_t m, unsigned seed) {
    if (m > n*(n-1)/2) throw std::invalid_argument("Too many edges for simple undirected graph.");
    Graph G(n);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<std::size_t> dist(0, n-1);

    std::unordered_set<unsigned long long> used; // encode edge (min,max)
    used.reserve(m*2 + 16);

    auto encode = [](std::size_t a, std::size_t b)->unsigned long long{
        if (a > b) std::swap(a, b);
        return (static_cast<unsigned long long>(a) << 32) | static_cast<unsigned long long>(b);
    };

    while (G.m() < m) {
        std::size_t u = dist(rng);
        std::size_t v = dist(rng);
        if (u == v) continue;
        auto key = encode(u, v);
        if (used.insert(key).second) {
            (void)G.add_edge(u, v);
        }
    }
    return G;
}

bool Graph::is_connected_ignoring_isolated() const {
    if (m_n == 0) return true;
    // Find a vertex with degree > 0
    std::size_t start = m_n;
    for (std::size_t i = 0; i < m_n; ++i) {
        if (!adj[i].empty()) { start = i; break; }
    }
    if (start == m_n) {
        // no edges: either empty or isolated nodes only -> Euler circuit exists only if n==0 or m==0 and deg all 0,
        // but by convention, an empty circuit is fine. Treat as connected.
        return true;
    }
    std::vector<bool> vis(m_n, false);
    std::queue<std::size_t> q;
    vis[start] = true; q.push(start);
    std::size_t reached = 0;
    while (!q.empty()) {
        auto u = q.front(); q.pop();
        ++reached;
        for (auto v : adj[u]) {
            if (!vis[v]) { vis[v] = true; q.push(v); }
        }
    }
    // Count vertices with degree>0
    std::size_t non_isolated = 0;
    for (std::size_t i = 0; i < m_n; ++i) if (!adj[i].empty()) ++non_isolated;
    return reached == non_isolated;
}

bool Graph::all_even_degrees() const {
    for (std::size_t i = 0; i < m_n; ++i) {
        if ((adj[i].size() % 2ULL) != 0ULL) return false;
    }
    return true;
}
