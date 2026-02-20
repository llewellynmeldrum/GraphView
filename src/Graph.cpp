#include "Graph.hpp"
#include "SharedContext.hpp"

void Graph::init(GraphInitConfig _init_cfg) {
    this->init_cfg = _init_cfg;

    const int& V = init_cfg.V;
    const int& E = init_cfg.E;
    adjList = std::vector<std::vector<Node>>(V);
    nodePositions = std::vector<glm::vec2>(V);
    nodeColors = std::vector<glm::vec4>(V, {1.0f, 1.0f, 1.0f, 1.0f});
    println("V:{}, E:{}", V, E);

    auto min = init_cfg.minPos;
    auto max = init_cfg.maxPos;
    println("Generating graph between bounds=[{},{}] and [{},{}]", min.x, min.y, max.x, max.y);

    // 1. Set a random position for each node
    for (Node u = 0; u < V; u++) {
        this->nodePositions[u] = randVec2(init_cfg.minPos, init_cfg.maxPos);
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
