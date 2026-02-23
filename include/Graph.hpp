#pragma once

#include "Colors.hpp"
#include "Vectors.hpp"
#include <cstddef>
#include <glm/glm.hpp>
#include <print>
#include <random>
#include <vector>

struct Graph;
struct GraphInitConfig {
    float     T0 = 100.0f;
    float     T1 = 10.0f;
    bool      noSelfEdges = true;
    bool      weighted = false;
    glm::vec2 xBounds = glm::vec2{-1000.0f, 1000.0f};
    glm::vec2 yBounds = glm::vec2{-1000.0f, 1000.0f};
    int       V = 100;
    int       E = 200;
};

struct GraphConfig {
    std::unique_ptr<Graph> ptr;
    bool                   isHidden = false;  //? scale? idk
    struct DrawSettings {
        float     nodeSizeWorld = 3.0f;
        glm::vec4 baseNodeColor = WHITE;
        glm::vec4 edgeColor = {1, 1, 1, 1};
        float     edgeTaperOutgoing = 0.3;
        float     edgeTaperIncoming = 0.3;
        bool      enableEdgeTapering = true;
        bool      showBounds = false;
    } draw;

    struct UpdateSettings {
        bool  isForceDirected = true;
        float timeScale = 1.0f;
        bool  isPaused = false;
        float temp;
    } update;
};
struct Graph {
    glm::vec2 clampToBounds(glm::vec2 v);
    float     averageEdgeLength{};
    using Node = int;
    void update(double dT);
    struct Edge {
        Node  u{0}, v{0};
        float weight = 1.0;
    };
    GraphInitConfig init_cfg;
    GraphConfig&    cfg;

    std::vector<std::vector<Node>> adjList;
    std::vector<glm::vec2>         nodePositions;     // (world positions)
    std::vector<glm::vec2>         nodeVelocity;      // (world positions)
    std::vector<glm::vec2>         nodeAcceleration;  // (world positions)
    std::vector<glm::vec4>         nodeColors;
    std::vector<bool>              isNodeColored;
    std::vector<Edge>              edges;

    Graph(GraphConfig& _cfg) : cfg(_cfg) {};

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
        for (const auto& p : nodePositions) {
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
    glm::vec2 randVec2(glm::vec2 min = {0, 0}, glm::vec2 max = {1, 1}) {
        return {rand(min.x, max.x), rand(min.y, max.y)};
    }
    Node getRandomNode() {
        return (Node)std::uniform_int_distribution<int>{0, init_cfg.V - 1}(_rand_engine);
    }
};
