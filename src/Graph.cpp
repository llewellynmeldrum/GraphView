#include "Graph.hpp"
#include "GLFWHandler.hpp"
#include "SharedContext.hpp"
#include "glm_wrapper.hpp"
#include <algorithm>
#include <imgui.h>

void Graph::init(SharedContext::GraphInitConfig _init_cfg) {
    this->init_cfg = _init_cfg;

    const int& V = init_cfg.V;
    const int& E = init_cfg.E;
    adjList = std::vector<std::vector<Node>>(V);
    worldPos = std::vector<vec2>(V);
    screenPos = std::vector<vec2>(V);
    id = std::vector<std::string>(V, "");
    degree = std::vector<int>(V);
    degreeFactor = std::vector<float>(V);
    isNodeColored = std::vector<bool>(V, false);
    nodeColor = std::vector<vec4>(V, {1.0f, 1.0f, 1.0f, 1.0f});
    println("V:{}, E:{}", V, E);

    vec2 min = {init_cfg.xBounds.x, init_cfg.yBounds.x};
    vec2 max = {init_cfg.xBounds.y, init_cfg.yBounds.y};
    cfg.update.currTemp = _init_cfg.initialTemperature;

    println("Generating graph between bounds=[{},{}] and [{},{}]", min.x, min.y, max.x, max.y);

    // 1. Set a random position for each node
    for (Node u = 0; u < V; u++) {
        worldPos[u] = randVec2(min, max);
        id[u] = std::format("{:02x}", u);
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
                if (!init_cfg.enableSelfEdges && u == v) continue;
                adjList[u].push_back(v);
                degree[u]++;
                degree[v]++;
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
            degree[u]++;
            degree[v]++;
        }
    }
    for (Node u = 0; u < V; u++) {
        degreeFactor[u] = static_cast<float>(1 - degree[u]);
    }
}
static inline vec2 clampMag(vec2 v, float maxLen) {
    float len2 = dot(v, v);
    if (len2 > maxLen * maxLen) {
        float invLen = 1.0f / sqrtf(len2);
        return v * (maxLen * invLen);
    }
    return v;
}

constexpr float eps = 0.00001f;

#define sq(x) static_cast<float>(pow(x, 2))

inline float boundedLengthSquared(const vec2& v, float min = 0.00001f) {
    return fmax(sq(v.x) + sq(v.y), min);
}
inline float boundedLength(const vec2& v, float min = 0.00001f) {
    return fmax(sqrt(sq(v.x) + sq(v.y)), min);
}

void Graph::updateGraph(double frame_dT) {
    if (!cfg.update.isForceDirected || cfg.update.isPaused) return;
    if (cfg.update.currTemp <= init_cfg.restingTemperature) {
        return;
    }

    const auto& V = init_cfg.V;
    const auto& E = init_cfg.E;
    const auto& attractionFactor = init_cfg.attractionFactor;
    const auto& repulsionFactor = init_cfg.repulsionFactor;
    const auto& substeps = init_cfg.substeps;
    const auto& coolingFactor = init_cfg.coolingFactor;
    auto&       pos = worldPos;

    std::vector<vec2> displacement(init_cfg.V);

    float area = (init_cfg.xBounds.y - init_cfg.xBounds.x)  //
                 * (init_cfg.yBounds.y - init_cfg.yBounds.x);

    float      k = sqrt(area / V);
    const auto k_squared = k * k;
    for (int step = 0; step < substeps; step++) {
        for (Node u = 0; u < V; u++) {
            // repulsion
            for (Node v = u + 1; v < V; v++) {
                const vec2  deltaP = pos[u] - pos[v];
                const float dist = boundedLength(deltaP);
                const vec2  direction = deltaP / dist;
                const float forceMag = k_squared / dist;  // force = C^2/dist for replusion

                const vec2 force = (direction * forceMag) * repulsionFactor;

                displacement[u] += force;  // pushing u away from v
                displacement[v] -= force;  // pushing v away from u
            }
            // attraction
            for (Node v : adjList[u]) {
                const vec2  deltaP = pos[u] - pos[v];
                const float dist = boundedLength(deltaP);
                const vec2  direction = deltaP / dist;
                const float forceMag = sq(dist) / k;  // force = dist^2/C for attraction
                const vec2  force = (direction * forceMag) * attractionFactor;

                displacement[u] -= force;  // pulling u towards v
                displacement[v] += force;  // pushing v towards u
            }
        }
        for (Node u = 0; u < V; u++) {
            float mag = length(displacement[u]);
            if (mag > 0) {
                displacement[u] /= mag;  // normalize
                mag = std::min(mag, cfg.update.currTemp);
                displacement[u] *= mag;  // unnormalize with clamped mag
                pos[u] += displacement[u];
            }
            clampToGraphBoundaries(pos[u]);
        }
    }
    cfg.update.currTemp *= coolingFactor;
}
void Graph::clampToGraphBoundaries(vec2& v) {
    using vec2 = vec2;
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
}
