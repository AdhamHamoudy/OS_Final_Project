#pragma once

#include <vector>
#include <string>
#include <optional>
#include <cstddef>

/**
 * Simple undirected graph with 0-based vertex IDs.
 * Constraints:
 *  - No self-loops
 *  - No parallel edges
 * Storage:
 *  - Adjacency lists in 'adj'
 *
 * Implemented in graph.cpp:
 *  - Graph(std::size_t n)
 *  - bool add_edge(std::size_t u, std::size_t v)
 *  - static std::optional<Graph> load_from_file(const std::string& path)
 *  - static Graph random_simple(std::size_t n, std::size_t m, unsigned seed)
 *  - bool is_connected_ignoring_isolated() const
 *  - bool all_even_degrees() const
 */
class Graph {
public:
    // ---- Construction ----
    explicit Graph(std::size_t n);

    // ---- Basic queries (inline) ----
    std::size_t n() const noexcept { return m_n; }
    std::size_t m() const noexcept { return m_m; }
    const std::vector<std::size_t>& neighbors(std::size_t u) const { return adj[u]; }

    // ---- Edge updates ----
    // Returns true if a new edge was added; false if invalid or already exists.
    bool add_edge(std::size_t u, std::size_t v);

    // ---- Euler helpers ----
    // True if the subgraph induced by non-isolated vertices is connected.
    bool is_connected_ignoring_isolated() const;

    // True if every vertex has even degree.
    bool all_even_degrees() const;

    // ---- I/O & generators ----
    // Load graph from file: first line "n m", then m lines "u v".
    // On parse/validation error, returns std::nullopt.
    static std::optional<Graph> load_from_file(const std::string& path);

    // Generate a random simple undirected graph with exactly m edges.
    // Throws std::invalid_argument if m > n*(n-1)/2 or parameters invalid.
    static Graph random_simple(std::size_t n, std::size_t m, unsigned seed);

private:
    std::size_t m_n{0};
    std::size_t m_m{0};
    std::vector<std::vector<std::size_t>> adj;
};
