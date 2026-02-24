#pragma once

#include "Colors.hpp"
#include "SharedContext.hpp"
#include "Vectors.hpp"
#include <cstddef>
#include <glm/glm.hpp>
#include <print>
#include <random>
#include <vector>

struct Graph;

struct Graph {
    Graph(SharedContext& _shared) : shared(_shared), cfg(shared.graphs) {};
    SharedContext& shared;
    double         T0;
    void           clampToGraphBoundaries(glm::vec2& v);
    float          averageEdgeLength{};
    using Node = int;
    void update(double dT);
    struct Edge {
        Node  u{0}, v{0};
        float weight = 1.0;
    };
    SharedContext::GraphInitConfig init_cfg;
    SharedContext::GraphConfig&    cfg;

    std::vector<std::vector<Node>> adjList;
    std::vector<glm::vec2>         nodePositions;
    std::vector<glm::vec2>         screenPos;
    std::vector<int>               degree;
    std::vector<float>             degreeFactor;  // log(degree[u])
    std::vector<glm::vec4>         nodeColors;
    std::vector<bool>              isNodeColored;
    std::vector<Edge>              edges;

    std::vector<std::string> id;  //<position, id>

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
