#pragma once

#include <cstddef>
#include <glm/glm.hpp>
#include <print>
#include <random>
#include <vector>

#include "Colors.hpp"
#include "SharedContext.hpp"
#include "Vectors.hpp"
#include "glm_wrapper.hpp"

struct Graph;

struct Graph {
    using Node = int;

    template <class T>
    using vector = std::vector<T>;

    Graph(SharedContext& _shared) : shared(_shared), cfg(shared.graphs) {};
    SharedContext& shared;
    void           updateGraph(double dT);
    struct Edge {
        Node  u{0}, v{0};
        float weight = 1.0;
    };
    SharedContext::GraphInitConfig init_cfg;
    SharedContext::GraphConfig&    cfg;

    vector<vector<Node>> adjList;
    vector<vec2>         worldPos;
    vector<vec2>         screenPos;
    vector<int>          degree;
    vector<float>        degreeFactor;  // log(degree[u]), used for scaling (not anymore)
    vector<bool>         isNodeColored;
    vector<vec4>         nodeColor;
    vector<Edge>         edges;

    vector<std::string> id;  //<position, id>

    void init(SharedContext::GraphInitConfig init_cfg);

    inline void debug_print_adj() {
        std::println("ADJACENCY LIST:");
        for (Node u = 0; u < init_cfg.V; u++) {
            std::print("{}: [", u);
            for (Node v : adjList[u]) {
                std::print("{}", v);
                if (v != *adjList[u].rend()) {
                    std::print(",");
                }
            }
            std::println("]");
        }
    }
    inline void debug_print_edges() {
        std::println("EDGE LIST:");
        for (Edge e : edges) {
            std::println("\t[{},{}] ", e.u, e.v);
        }
    }
    inline void debug_print_positions() {
        std::println("POSITIONS :");
        int i = 0;
        for (const auto& p : worldPos) {
            std::println("\t{} = [{},{}] ", i++, p.x, p.y);
        }
    }

 private:
    void clampToGraphBoundaries(vec2& v);

    // Randomization
    std::random_device randDevice{};
    std::mt19937       randEngine{randDevice()};

    float rand(float min = 0.0f, float max = 1.0f) {
        return std::uniform_real_distribution<float>{min, max}(randEngine);
    }
    vec2 randVec2(vec2 min = {0, 0}, vec2 max = {1, 1}) {
        return {rand(min.x, max.x), rand(min.y, max.y)};
    }
    Node getRandomNode() { return std::uniform_int_distribution{0, init_cfg.V - 1}(randEngine); }
};
