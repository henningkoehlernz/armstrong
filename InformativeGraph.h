#ifndef INFORMATIVE_GRAPH_H
#define INFORMATIVE_GRAPH_H

#include <string>
#include <iostream>
#include <limits.h>
#include <boost/dynamic_bitset.hpp>
#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <unordered_set>
#include "AgreeSetTypes.h"

typedef uint32_t NodeID;
typedef uint32_t AgreeSetID;

/**
 * undirected graph with tuples as vertices and agree-sets as edge labels
 * goal is to identify a minimal vertex subset that induces a subgraph preserving all labels
 */
class InformativeGraph
{
private:
    typedef std::pair<NodeID,NodeID> Edge;
    // nodes store list of neighbors
    std::vector<std::vector<NodeID>> neighbors;
    // edge labels are stored separately
    std::unordered_map<Edge,AgreeSetID,boost::hash<Edge>> edgeLabels;
    // track which nodes have been already been picked
    boost::dynamic_bitset<> pickedNodes;

    // validate that graph is consistent, returning error message in msg
    bool validate(std::string &msg) const;
    bool hasEdge(NodeID v, NodeID w) const;

    void removeEdge(NodeID v, NodeID w);
    void removeAgreeSet(AgreeSetID ag, std::unordered_set<NodeID> *updated);

public:
    InformativeGraph(size_t nodeCount = 0);

    size_t nodeCount() const;
    size_t degree(NodeID node) const;
    AgreeSetID getEdgeLabel(NodeID v, NodeID w) const;
    bool picked(NodeID node) const;

    void addEdge(NodeID v, NodeID w, AgreeSetID ag);
    /**
     * marks node a picked; for each edge to other picked node removes all edges with same label
     * if updated is set, inserts all nodes adjacent to edges removed
     */
    virtual void pickNode(NodeID node, std::unordered_set<NodeID> *updated = nullptr);
    virtual void removeNode(NodeID node);

    std::vector<NodeID> getNeighbors(NodeID node) const;
    // get AgreeSetIDs on adjacent edges, in order
    std::vector<AgreeSetID> getPossibleAgreeSets(NodeID node) const;
    // get AgreeSetIDs on adjacent edges to picked nodes, in order
    std::vector<AgreeSetID> getCertainAgreeSets(NodeID node) const;
    // returns nodes that appear in all edges labeled with some particular agree-set
    std::vector<NodeID> getForced() const;
    // returns all nodes picked
    std::vector<NodeID> getPicked() const;

    friend std::ostream& operator<<(std::ostream &os, const InformativeGraph &g);
};

#endif
