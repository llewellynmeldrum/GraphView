#pragma once

#include "Vectors.hpp"
#include <cstddef>
#include <print>
#include <random>
#include <vector>

struct Graph;
struct GraphInitConfig {
    bool noSelfEdges = true;
    bool weighted = false;
    Vec2 minPos;
    Vec2 maxPos;
    int  V = 1;
    int  E = 0;
};

struct GraphConfig {
    std::unique_ptr<Graph> ptr;
    bool                   isHidden = false;  //? scale? idk
    struct DrawSettings {
        float NodeSize_px = 50;  // diameter in pixels
        float EdgeWidth_px = 5;
        float edgeMargin = 0.1f;
        // eventually
        // float taperStart_px = 10;
        // float taperEnd_px = 2;

    } draw;
};
struct Graph {
    using Node = int;
    struct Edge {
        Node  u{0}, v{0};
        float weight = 1.0;
    };
    GraphInitConfig init_cfg;

    std::vector<std::vector<Node>> adjList;
    std::vector<Vec2>              nodePositions;  // euclidean position of nodes.
    std::vector<Edge>              edges;

    Graph() {};

    void init(GraphInitConfig init_cfg);
    void reset(auto init_cfg) { init(init_cfg); }

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
        for (Vec2 p : nodePositions) {
            std::println("\t{} = [{},{}] ", i++, p.x, p.y);
        }
    }

 private:
    // Randomization
    std::random_device rd{};
    std::mt19937       _rand_engine{rd()};

    float rand(float min = 0.0f, float max = 1.0f) {
        return std::uniform_real_distribution<float>{min, max}(_rand_engine);
    }
    Vec2 randVec2(Vec2 min = {0, 0}, Vec2 max = {1, 1}) {
        return {rand(min.x, max.x), rand(min.y, max.y)};
    }
    Node getRandomNode() {
        return (Node)std::uniform_int_distribution<int>{0, init_cfg.V}(_rand_engine);
    }
};
