#ifndef AGREE_SET_GRAPH_H
#define AGREE_SET_GRAPH_H

#include <string>
#include <iostream>
#include <limits.h>
#include "AgreeSetTypes.h"

typedef uint16_t NodeID;
typedef uint16_t AttID;
typedef uint16_t EdgeID;

#define MAX_NODE 128

// utility functions
bool operator<=(const AttributeSet &a, const AttributeSet &b);
std::vector<NodeID> diff(const AttributeSet &a, const AttributeSet &b);

class ClosureOp
{
public:
    virtual AttributeSet operator()(const AttributeSet &a) const = 0;
};

class GenClosureOp : public ClosureOp
{
    std::vector<AttributeSet> generators;
public:
    static std::vector<AttributeSet> getGenerators(const std::vector<AttributeSet> &agreeSets);
    virtual AttributeSet operator()(const AttributeSet &a) const;
    GenClosureOp(const std::vector<AttributeSet> &generators);
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
        EdgeData(size_t size);
        EdgeData(const EdgeData &e);
    };
    typedef std::vector<NodeID> Partition;
    // Isomorphism type of nodes
    enum class Connected : int8_t { None=0, TwoPlus=2, Pre=-1, Post=1 };

    // agree-sets associated with edges
    std::vector<EdgeData> edges;
    // store connected components for each attribute graph (these form cliques)
    std::vector<Partition> attComp;
    // store if nodes have isomorphic nodes (for pruning)
    std::vector<Connected> isoType;

    // update isomorphism type for node and isomorphic nodes to 2+
    void setIsoTwoPlus(NodeID node);
    // validate that graph is consistent, returning error message in msg
    bool validate(std::string &msg) const;

    friend std::ostream& operator<<(std::ostream &os, const EdgeData &edge);
public:
    AgreeSetGraph(size_t nodeCount, size_t attCount);
    AgreeSetGraph(const AgreeSetGraph &g);
    size_t nodeCount() const;
    size_t attributeCount() const;
    // number of nodes adjacent to assigned edge
    size_t activeNodeCount() const;
    // shrink graph to active size
    void shrinkToActive();
    // get attribute set for given edge
    const AttributeSet& at(NodeID a, NodeID b) const;
    // quick test whether agreeSet might be assignable to (a,b)
    bool canAssign(NodeID a, NodeID b, AttributeSet agreeSet) const;
    // try to assign agreeSet to (a,b)
    bool assign(NodeID a, NodeID b, AttributeSet agreeSet, const ClosureOp &closure);
    // construct Armstrong table represented by agree-set graph
    std::vector<std::vector<int>> toArmstrongTable() const;

    friend std::ostream& operator<<(std::ostream &os, const AgreeSetGraph &g);
};

// main function, btLimit imposes limit on number of back-tracking steps
AgreeSetGraph findMinAgreeSetGraph(const std::vector<AttributeSet> &agreeSets, unsigned int btLimit = UINT_MAX);

#endif
