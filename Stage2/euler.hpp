#ifndef EULER_HPP
#define EULER_HPP

#include "graph.hpp"
#include <vector>
#include <string>

struct EulerCheck {
    bool ok;
    std::string reason;
};

// Check if Euler circuit exists
EulerCheck euler_feasibility(const Graph& G);

// Return Euler circuit (possibly empty if infeasible)
std::vector<std::size_t> find_euler_circuit(const Graph& G);

#endif // EULER_HPP
