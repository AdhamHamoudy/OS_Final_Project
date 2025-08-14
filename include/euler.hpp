#pragma once
#include "graph.hpp"
#include <vector>

// Returns empty vector if no Euler circuit exists.
// Pre-conditions (for non-empty result): connected (ignoring isolated) + all degrees even.
// Uses Hierholzer’s algorithm, O(m).
std::vector<std::size_t> find_euler_circuit(const Graph& G);

// Convenience: returns {ok, reason}. If ok=false, reason explains the failure.
struct EulerCheck {
    bool ok;
    const char* reason;
};
EulerCheck euler_feasibility(const Graph& G);
