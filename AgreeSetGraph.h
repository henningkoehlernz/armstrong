#ifndef AGREE_SET_GRAPH_H
#define AGREE_SET_GRAPH_H

#include <bits/stdc++.h>
using namespace std;

typedef uint8_t NodeID;
typedef uint16_t EdgeID;

// attributes are encoded as integers 0..k
typedef vector<bool> AgreeSet;
// for tracking connected components of attribute graphs
typedef vector<vector<NodeID>> Partition;

struct EdgeData
{
    AgreeSet att;
    bool forced, checked;
};

class ClosureOp
{
public:
    virtual AgreeSet operator()(const AgreeSet &a) const = 0;
};

/**
 * Simplex with edges annotated with agree-sets
 * used for tracking agree-set assignment and near-cycle checking
 */
class AgreeSetGraph
{
    // convert vertex-pair (=edge) to integer, independent of number of nodes
    static EdgeID edge(NodeID a, NodeID b);

    // agree-sets associated with edges
    vector<EdgeData> edges;
    // store connected components for each attribute graph (these form cliques)
    vector<Partition> attComp;

    // function for computing closure
    const ClosureOp &closure;
public:
    AgreeSetGraph(size_t nodeCount, size_t attCount, const ClosureOp &closure);
    AgreeSetGraph(const AgreeSetGraph &g);
    const AgreeSet& at(NodeID a, NodeID b) const;
    bool assign(NodeID a, NodeID b, AgreeSet value);
};

#endif
