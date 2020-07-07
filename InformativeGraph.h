#ifndef INFORMATIVE_GRAPH_H
#define INFORMATIVE_GRAPH_H

#include <string>
#include <iostream>
#include <limits.h>
#include <boost/dynamic_bitset.hpp>
#include <boost/functional/hash.hpp>
#include <unordered_map>
#include "AgreeSetTypes.h"

typedef uint16_t NodeID;
typedef uint16_t AgreeSetID;

/**
 * undirected graph with tuples as vertices and agree-sets as edge labels
 * goal is to identify a minimal vertex subset that induces a subgraph preserving all labels
 */
class InformativeGraph
{
private:
    // nodes store list of neighbors
    std::vector<std::vector<NodeID>> neighbors;
    // edge labels are stored separately
    std::unordered_map<std::pair<NodeID,NodeID>, AgreeSetID, boost::hash<std::pair<NodeID,NodeID>>> edgeLabels;
    // track which nodes have been already been picked
    boost::dynamic_bitset<> pickedNodes;

    // validate that graph is consistent, returning error message in msg
    bool validate(std::string &msg) const;

    void removeEdge(NodeID v, NodeID w);
    void removeAgreeSet(AgreeSetID ag);

public:
    InformativeGraph(size_t nodeCount = 0);

    size_t nodeCount() const;
    size_t degree(NodeID node) const;
    AgreeSetID getEdgeLabel(NodeID v, NodeID w) const;
    bool picked(NodeID node) const;

    void addEdge(NodeID v, NodeID w, AgreeSetID ag);
    void pickNode(NodeID node);
    void removeNode(NodeID node);

    std::vector<NodeID> getNeighbors(NodeID node) const;
    // get AgreeSetIDs on adjacent edges
    std::vector<AgreeSetID> getPossibleAgreeSets(NodeID node) const;
    // get AgreeSetIDs on adjacent edges to picked nodes
    std::vector<AgreeSetID> getCertainAgreeSets(NodeID node) const;

    friend std::ostream& operator<<(std::ostream &os, const InformativeGraph &g);
};

#endif
