#pragma once
#include <vector>
#include <string>
#include <optional>
#include <cstddef>

class Graph {
public:
    explicit Graph(std::size_t n = 0);                 // undirected simple graph
    std::size_t n() const noexcept { return m_n; }
    std::size_t m() const noexcept { return m_m; }

    bool add_edge(std::size_t u, std::size_t v);       // no self-loops, no multi-edges
    const std::vector<std::size_t>& neighbors(std::size_t u) const { return adj[u]; }
    std::size_t degree(std::size_t u) const { return adj[u].size(); }

    static std::optional<Graph> load_from_file(const std::string& path);
    static Graph random_simple(std::size_t n, std::size_t m, unsigned seed);

    // Utilities used by Euler check:
    bool is_connected_ignoring_isolated() const;       // BFS/DFS from first non-isolated node
    bool all_even_degrees() const;

private:
    std::size_t m_n{};
    std::size_t m_m{};
    std::vector<std::vector<std::size_t>> adj;
};
