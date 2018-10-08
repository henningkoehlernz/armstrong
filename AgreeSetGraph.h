#ifndef AGREE_SET_GRAPH_H
#define AGREE_SET_GRAPH_H

#include <bits/stdc++.h>
#include <boost/log/trivial.hpp>
using namespace std;

typedef uint8_t NodeID;
typedef uint8_t AttID;
typedef uint16_t EdgeID;

// attributes are encoded as integers 0..k
#define MAX_ATT 4
#define MAX_NODE 4
typedef bitset<MAX_ATT> AttributeSet;

class ClosureOp
{
public:
    virtual AttributeSet operator()(const AttributeSet &a) const = 0;
};

class GenClosureOp : public ClosureOp
{
    vector<AttributeSet> generators;
public:
    static vector<AttributeSet> getGenerators(const vector<AttributeSet> &agreeSets);
    virtual AttributeSet operator()(const AttributeSet &a) const;
    GenClosureOp(const vector<AttributeSet> &generators);
    GenClosureOp(const GenClosureOp &op);
};

/**
 * Simplex with edges annotated with agree-sets
 * used for tracking agree-set assignment and near-cycle checking
 */
class AgreeSetGraph
{
public:
    // convert node-pair to integer, independent of number of nodes in graph
    static EdgeID toEdge(NodeID a, NodeID b);
    // convert edgeID to pair of nodes
    static void toNodes(EdgeID e, NodeID &a, NodeID &b);
private:
    struct EdgeData
    {
        AttributeSet attSet;
        bool assigned; // does the edge store an assigned agree-set?
        EdgeData();
        EdgeData(const EdgeData &e);
    };
    typedef vector<NodeID> Partition;

    // agree-sets associated with edges
    vector<EdgeData> edges;
    // store connected components for each attribute graph (these form cliques)
    vector<Partition> attComp;

    friend ostream& operator<<(ostream &os, const EdgeData &edge);
public:
    AgreeSetGraph(size_t nodeCount, size_t attCount);
    AgreeSetGraph(const AgreeSetGraph &g);
    const AttributeSet& at(NodeID a, NodeID b) const;
    // quick test whether agreeSet might be assignable to (a,b)
    bool canAssign(NodeID a, NodeID b, AttributeSet agreeSet) const;
    // try to assign agreeSet to (a,b)
    bool assign(NodeID a, NodeID b, AttributeSet agreeSet, const ClosureOp &closure);

    friend ostream& operator<<(ostream &os, const AgreeSetGraph &g);
};

AgreeSetGraph findMinAgreeSetGraph(const vector<AttributeSet> &agreeSets);

#endif
