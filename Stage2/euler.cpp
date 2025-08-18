#include "euler.hpp"
#include <stack>
#include <algorithm> // for std::reverse

EulerCheck euler_feasibility(const Graph& G) {
    if (!G.is_connected_ignoring_isolated())
        return {false, "Graph is not connected when ignoring isolated vertices"};
    if (!G.all_even_degrees())
        return {false, "Not all vertices have even degree"};
    return {true, "OK"};
}

// Hierholzer’s algorithm for undirected graphs
std::vector<std::size_t> find_euler_circuit(const Graph& G) {
    auto chk = euler_feasibility(G);
    if (!chk.ok) return {};

    const std::size_t n = G.n();
    if (n == 0) return {};        // no vertices, no circuit
    if (G.m() == 0) return {};    // no edges -> empty tour (assignment-friendly)

    // Copy adjacency
    std::vector<std::vector<std::size_t>> adj(n);
    for (std::size_t u = 0; u < n; ++u)
        adj[u] = G.neighbors(u);

    std::vector<std::size_t> out;
    out.reserve(G.m() + 1);

    // Start vertex: first with edges (guaranteed to exist since m>0)
    std::size_t start = 0;
    for (std::size_t i = 0; i < n; ++i)
        if (!adj[i].empty()) { start = i; break; }

    std::stack<std::size_t> st;
    st.push(start);

    auto remove_edge = [&](std::size_t u, std::size_t v){
        auto& Au = adj[u];
        for (std::size_t i = 0; i < Au.size(); ++i) {
            if (Au[i] == v) { Au[i] = Au.back(); Au.pop_back(); break; }
        }
        auto& Av = adj[v];
        for (std::size_t i = 0; i < Av.size(); ++i) {
            if (Av[i] == u) { Av[i] = Av.back(); Av.pop_back(); break; }
        }
    };

    while (!st.empty()) {
        std::size_t u = st.top();
        if (!adj[u].empty()) {
            std::size_t v = adj[u].back();
            adj[u].pop_back();
            remove_edge(u, v);
            st.push(v);
        } else {
            out.push_back(u);
            st.pop();
        }
    }

    std::reverse(out.begin(), out.end());
    return out;
}
