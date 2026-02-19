#include "Graph.hpp"
#include "SharedContext.hpp"

void Graph::init() {
    const int& V = init_cfg.V;
    const int& E = init_cfg.E;
    adjList = std::vector<std::vector<Node>>(V, std::vector<Node>());
    nodePositions = std::vector<Vec2>(V, Vec2());

    auto [minx, miny] = init_cfg.minPos;
    auto [maxx, maxy] = init_cfg.maxPos;
    println("Generating graph between bounds=[{},{}] and [{},{}]", minx, miny, maxx, maxy);

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
        // try every node until one gets it, or we do a full iteration.
        bool foundNode = false;
        for (Node u = 0; u < V; u++) {
            float threshold = oddsOfReceivingEdge(u);
            if (Graph::rand() < threshold) {
                Node v = getRandomNode();
                if (init_cfg.noSelfEdges && u == v) continue;
                adjList[u].push_back(v);
                edges.push_back({u, v});
                // succesfully added an edge, break
                foundNode = true;
                break;
            }
        }
        // We iterated through the whole list without a match, just pair up two random nodes
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
