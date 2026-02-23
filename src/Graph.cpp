#include "Graph.hpp"
#include "SharedContext.hpp"
#include <algorithm>

void Graph::init(GraphInitConfig _init_cfg) {
    this->init_cfg = _init_cfg;

    const int& V = init_cfg.V;
    const int& E = init_cfg.E;
    adjList = std::vector<std::vector<Node>>(V);
    nodePositions = std::vector<glm::vec2>(V);
    nodeVelocity = std::vector<glm::vec2>(V);
    nodeAcceleration = std::vector<glm::vec2>(V);
    isNodeColored = std::vector<bool>(V, false);
    nodeColors = std::vector<glm::vec4>(V, {1.0f, 1.0f, 1.0f, 1.0f});
    println("V:{}, E:{}", V, E);

    glm::vec2 min = {init_cfg.xBounds.x, init_cfg.yBounds.x};
    glm::vec2 max = {init_cfg.xBounds.y, init_cfg.yBounds.y};
    cfg.update.temp = _init_cfg.T0;

    println("Generating graph between bounds=[{},{}] and [{},{}]", min.x, min.y, max.x, max.y);

    // 1. Set a random position for each node
    for (Node u = 0; u < V; u++) {
        this->nodePositions[u] = randVec2(min, max);
    }
    if (E > 2 * V) {
        println("Warning! EdgeCount is greater than 2*NodeCount. This may slow down "
                "generation.");
    }
    auto oddsOfReceivingEdge = [this](Node u) -> float {
        int outDegree = adjList[u].size();
        return 1.0f / (outDegree + 1.0f);
    };
    // 2. Assign edges to random nodes according to above probability
    // i.e the more outgoing edges, the less likely a node is to get another.
    for (size_t ei = 0; ei < E; ei++) {
        // a nodes chance to get an edge = 1/(outDegree+1)
        // try a random node V times.
        bool foundNode = false;
        for (Node u = getRandomNode(); u < V; u = getRandomNode()) {
            float threshold = oddsOfReceivingEdge(u);
            if (Graph::rand() < threshold) {
                Node v = getRandomNode();
                if (init_cfg.noSelfEdges && u == v) continue;
                adjList[u].push_back(v);
                edges.push_back({u, v});
                foundNode = true;
                break;
            }
        }
        if (!foundNode) {
            Node u = getRandomNode();
            Node v = getRandomNode();
            while (u == v) {
                v = getRandomNode();
            }
            adjList[u].push_back(v);
            edges.push_back({u, v});
        }
    }
}
static inline glm::vec2 clampLen(glm::vec2 v, float maxLen) {
    float len2 = glm::dot(v, v);
    if (len2 > maxLen * maxLen) {
        float invLen = 1.0f / sqrtf(len2);
        return v * (maxLen * invLen);
    }
    return v;
}
constexpr float COOLING_FACTOR = 0.95;

glm::vec2 Graph::clampToBounds(glm::vec2 v) {
    if (v.x < init_cfg.xBounds.x) {
        v.x = init_cfg.xBounds.x;
    }
    if (v.x > init_cfg.xBounds.y) {
        v.x = init_cfg.xBounds.y;
    }
    if (v.y < init_cfg.yBounds.x) {
        v.y = init_cfg.yBounds.x;
    }
    if (v.y > init_cfg.yBounds.y) {
        v.y = init_cfg.yBounds.y;
    }
    return v;
}
void Graph::update(double frame_dT) {
    if (cfg.update.temp <= init_cfg.T1) {
        return;
    }
    float eps = 0.00001f;
    if (!cfg.update.isForceDirected || cfg.update.isPaused) return;
    auto&                  pos = nodePositions;
    std::vector<glm::vec2> disp = std::vector<glm::vec2>(init_cfg.V);

    float area = init_cfg.xBounds.y - init_cfg.xBounds.x;
    area *= init_cfg.yBounds.y - init_cfg.yBounds.x;
    float k = sqrt(area / init_cfg.V);
    println("{}", cfg.update.temp);
    for (Node u = 0; u < init_cfg.V; u++) {
        // repulsion
        for (Node v = u + 1; v < init_cfg.V; v++) {
            const glm::vec2 delta = pos[u] - pos[v];
            const float     d = std::max(glm::length(delta), eps);
            const glm::vec2 dir = delta / d;
            const float     f = (k * k) / d;
            const glm::vec2 F = dir * f;
            disp[u] += F;
            disp[v] -= F;
        }
        // attraction
        for (Node v : adjList[u]) {
            const glm::vec2 delta = pos[u] - pos[v];
            const float     d = std::max(glm::length(delta), eps);
            const glm::vec2 dir = delta / d;
            const float     f = (d * d) / k;
            const glm::vec2 F = dir * f;
            disp[u] -= F;
            disp[v] += F;
        }
    }
    for (Node u = 0; u < init_cfg.V; u++) {
        float mag = glm::length(disp[u]);
        if (mag > 0) pos[u] += (disp[u] / mag) * std::min(mag, cfg.update.temp);
        pos[u] = clampToBounds(pos[u]);
    }
    cfg.update.temp *= COOLING_FACTOR;  // e.g. 0.95
}
