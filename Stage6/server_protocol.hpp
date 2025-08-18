#pragma once
#include <string>
#include <vector>
#include <sstream>

inline std::vector<std::string> split_ws(const std::string& s) {
    std::istringstream is(s);
    std::vector<std::string> out; std::string t;
    while (is >> t) out.push_back(t);
    return out;
}
inline std::string join_sp(const std::vector<size_t>& xs) {
    std::ostringstream os;
    for (size_t i = 0; i < xs.size(); ++i) { if (i) os << ' '; os << xs[i]; }
    return os.str();
}
