#ifndef AGREE_SET_GRAPH_H
#define AGREE_SET_GRAPH_H

#include <bits/stdc++.h>
using namespace std;

typedef uint8_t NodeID;
typedef uint8_t AttID;
typedef uint16_t EdgeID;

// attributes are encoded as integers 0..k
typedef vector<bool> AttributeSet;

class ClosureOp
{
public:
    virtual AttributeSet operator()(const AttributeSet &a) const = 0;
};

/**
 * Simplex with edges annotated with agree-sets
 * used for tracking agree-set assignment and near-cycle checking
 */
class AgreeSetGraph
{
    // convert node-pair to integer, independent of number of nodes in graph
    static EdgeID toEdge(NodeID a, NodeID b);
    // convert edgeID to pair of nodes
    static void toNodes(EdgeID e, NodeID &a, NodeID &b);

    struct EdgeData
    {
        AttributeSet attSet;
        bool assigned; // does the edge store an assigned agree-set?
        EdgeData(size_t attCount);
        EdgeData(const EdgeData &e);
    };
    typedef vector<NodeID> Partition;

    // agree-sets associated with edges
    vector<EdgeData> edges;
    // store connected components for each attribute graph (these form cliques)
    vector<Partition> attComp;

    // function for computing closure
    const ClosureOp &closure;
public:
    AgreeSetGraph(size_t nodeCount, size_t attCount, const ClosureOp &closure);
    AgreeSetGraph(const AgreeSetGraph &g);
    const AttributeSet& at(NodeID a, NodeID b) const;
    bool assign(NodeID a, NodeID b, AttributeSet agreeSet);
};

#endif
