#pragma once
#include "../Stage1/graph.hpp"

struct GraphAlgorithm {
    virtual std::string name() const = 0;
    virtual std::string run(const Graph& G) = 0;
    virtual ~GraphAlgorithm() = default;
};

GraphAlgorithm* create_algorithm(const std::string& alg_name);
