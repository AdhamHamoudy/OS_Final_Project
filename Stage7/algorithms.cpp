#include "algorithms.hpp"
#include "euler.hpp"
#include <sstream>
#include <vector>
#include <queue>
#include <algorithm>

// ================= Euler Circuit (reuse Stage2) =================
class EulerCircuitAlg : public GraphAlgorithm {
public:
    std::string name() const override { return "EULER"; }
    std::string run(const Graph& G) override {
        auto chk = euler_feasibility(G);
        if (!chk.ok) return "ERR " + chk.reason;
        auto tour = find_euler_circuit(G);
        std::ostringstream os;
        os << "OK CIRCUIT " << (tour.size()-1) << "\n";
        for (size_t i = 0; i < tour.size(); ++i) {
            if (i) os << " ";
            os << tour[i];
        }
        return os.str();
    }
};

// ================= MST Weight (Kruskal, weight=1 edges) =================
class MstWeightAlg : public GraphAlgorithm {
public:
    std::string name() const override { return "MST"; }
    std::string run(const Graph& G) override {
        size_t n = G.num_vertices();

        struct Edge { size_t u, v; int w; };
        std::vector<Edge> edges;
        for (size_t u = 0; u < n; ++u) {
            for (size_t v : G.neighbors(u)) {
                if (u < v) edges.push_back({u, v, 1});
            }
        }

        // DSU with size_t
        std::vector<size_t> p(n);
        for (size_t i = 0; i < n; i++) p[i] = i;

        auto find = [&](size_t x) {
            while (p[x] != x) x = p[x];
            return x;
        };
        auto unite = [&](size_t a, size_t b) {
            a = find(a); b = find(b);
            if (a != b) p[a] = b;
        };

        int total = 0;
        for (auto &e : edges) {
            if (find(e.u) != find(e.v)) {
                unite(e.u, e.v);
                total += e.w;
            }
        }

        std::ostringstream os;
        os << "OK MST_WEIGHT " << total;
        return os.str();
    }
};

// ================= SCC (Kosaraju) =================
class SccAlg : public GraphAlgorithm {
public:
    std::string name() const override { return "SCC"; }

    void dfs1(const Graph& G, size_t u, std::vector<int>& vis, std::vector<size_t>& order) {
        vis[u] = 1;
        for (size_t v : G.neighbors(u)) if (!vis[v]) dfs1(G, v, vis, order);
        order.push_back(u);
    }

    void dfs2(const Graph& GT, size_t u, std::vector<int>& vis, std::vector<size_t>& comp) {
        vis[u] = 1; comp.push_back(u);
        for (size_t v : GT.neighbors(u)) if (!vis[v]) dfs2(GT, v, vis, comp);
    }

    std::string run(const Graph& G) override {
        size_t n = G.num_vertices();
        std::vector<int> vis(n, 0);
        std::vector<size_t> order;

        for (size_t i = 0; i < n; i++) if (!vis[i]) dfs1(G, i, vis, order);

        // Build transpose
        Graph GT(n);
        for (size_t u = 0; u < n; u++) for (size_t v : G.neighbors(u)) GT.add_edge(v, u);

        std::fill(vis.begin(), vis.end(), 0);
        std::ostringstream os;
        os << "OK SCC\n";

        for (int i = (int)order.size() - 1; i >= 0; i--) {
            size_t u = order[i];
            if (!vis[u]) {
                std::vector<size_t> comp;
                dfs2(GT, u, vis, comp);
                for (size_t j = 0; j < comp.size(); j++) {
                    if (j) os << " ";
                    os << comp[j];
                }
                os << "\n";
            }
        }
        return os.str();
    }
};

// ================= Max Flow (Edmonds-Karp, capacity=1) =================
class MaxFlowAlg : public GraphAlgorithm {
public:
    std::string name() const override { return "MAXFLOW"; }
    std::string run(const Graph& G) override {
        size_t n = G.num_vertices();
        std::vector<std::vector<int>> cap(n, std::vector<int>(n, 0));

        for (size_t u = 0; u < n; u++) 
            for (size_t v : G.neighbors(u)) 
                cap[u][v] = 1;

        int s = 0, t = (int)n - 1;
        int flow = 0;

        while (true) {
            std::vector<int> par(n, -1);
            std::queue<int> q; q.push(s); par[s] = s;
            while (!q.empty() && par[t] == -1) {
                int u = q.front(); q.pop();
                for (int v = 0; v < (int)n; v++) if (par[v] == -1 && cap[u][v] > 0) {
                    par[v] = u; q.push(v);
                }
            }
            if (par[t] == -1) break;

            int aug = 1e9;
            for (int v = t; v != s; v = par[v]) aug = std::min(aug, cap[par[v]][v]);
            for (int v = t; v != s; v = par[v]) {
                cap[par[v]][v] -= aug;
                cap[v][par[v]] += aug;
            }
            flow += aug;
        }

        std::ostringstream os;
        os << "OK MAXFLOW " << flow;
        return os.str();
    }
};

// ================= Hamiltonian Circuit (backtracking) =================
class HamiltonAlg : public GraphAlgorithm {
public:
    std::string name() const override { return "HAMILTON"; }

    bool dfs(const Graph& G, std::vector<int>& path, std::vector<int>& used, int n) {
        if ((int)path.size() == n) {
            int u = path.back(), v = path.front();
            for (size_t x : G.neighbors(u)) if ((int)x == v) return true;
            return false;
        }
        int u = path.back();
        for (size_t v : G.neighbors(u)) {
            if (!used[v]) {
                used[v] = 1; path.push_back((int)v);
                if (dfs(G, path, used, n)) return true;
                path.pop_back(); used[v] = 0;
            }
        }
        return false;
    }

    std::string run(const Graph& G) override {
        int n = (int)G.num_vertices();
        std::vector<int> used(n, 0), path;
        path.push_back(0); used[0] = 1;
        if (dfs(G, path, used, n)) {
            std::ostringstream os;
            os << "OK HAMILTON ";
            for (size_t i = 0; i < path.size(); i++) { if (i) os << " "; os << path[i]; }
            os << " " << path[0];
            return os.str();
        }
        return "ERR No Hamiltonian cycle";
    }
};

// ================= Factory =================
GraphAlgorithm* create_algorithm(const std::string& alg_name) {
    if (alg_name == "EULER") return new EulerCircuitAlg();
    if (alg_name == "MST") return new MstWeightAlg();
    if (alg_name == "SCC") return new SccAlg();
    if (alg_name == "MAXFLOW") return new MaxFlowAlg();
    if (alg_name == "HAMILTON") return new HamiltonAlg();
    return nullptr;
}
