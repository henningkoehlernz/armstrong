#include <unordered_set>
#include <boost/format.hpp>

#include "InformativeGraph.h"
#include "VectorUtil.h"

// local utility function
static std::pair<NodeID,NodeID> ordered(NodeID v, NodeID w)
{
    return v < w ? std::pair(v,w) : std::pair(w,v);
}

bool InformativeGraph::validate(std::string &msg) const
{
    for ( NodeID node = 0; node < nodeCount(); node++ )
        for ( NodeID neighbor : neighbors[node] )
        {
            if ( !contains(neighbors[neighbor], node) )
            {
                msg = (boost::format("node %1% is missing neighbor %2%") % neighbor % node).str();
                return false;
            }
        }
    return true;
}

void InformativeGraph::removeEdge(NodeID v, NodeID w)
{
    std::erase(neighbors[v], w);
    std::erase(neighbors[w], v);
}

void InformativeGraph::removeAgreeSet(AgreeSetID ag)
{
    for ( std::pair<std::pair<NodeID,NodeID>, AgreeSetID> keyValue : edgeLabels )
        if ( keyValue.second == ag )
            removeEdge(keyValue.first.first, keyValue.first.second);
}

InformativeGraph::InformativeGraph(size_t nodeCount) : neighbors(nodeCount), pickedNodes(nodeCount)
{
}

size_t InformativeGraph::nodeCount() const
{
    return neighbors.size();
}

size_t InformativeGraph::degree(NodeID node) const
{
    return neighbors[node].size();
}

AgreeSetID InformativeGraph::getEdgeLabel(NodeID v, NodeID w) const
{
    return edgeLabels.at(ordered(v,w));
}

bool InformativeGraph::picked(NodeID node) const
{
    return pickedNodes[node];
}

void InformativeGraph::addEdge(NodeID v, NodeID w, AgreeSetID ag)
{
    // store edge label
    std::pair<NodeID, NodeID> edge = ordered(v,w);
    assert(edgeLabels.count(edge) == 0);
    edgeLabels[edge] = ag;
    // resize graph if needed
    if ( neighbors.size() <= edge.second )
    {
        neighbors.resize(edge.second + 1);
        pickedNodes.resize(edge.second + 1);
    }
    // insert neighbors
    neighbors[v].push_back(w);
    neighbors[w].push_back(v);
}

void InformativeGraph::pickNode(NodeID node)
{
    assert(!pickedNodes[node]);
    pickedNodes[node] = true;
    // automatically eliminate edges that are no longer needed
    for ( AgreeSetID ag : getCertainAgreeSets(node) )
        removeAgreeSet(ag);
}

void InformativeGraph::removeNode(NodeID node)
{
    for ( NodeID neighbor : neighbors[node] )
        removeEdge(node, neighbor);
}

std::vector<NodeID> InformativeGraph::getNeighbors(NodeID node) const
{
    return neighbors[node];
}

std::vector<AgreeSetID> InformativeGraph::getPossibleAgreeSets(NodeID node) const
{
    std::unordered_set<AgreeSetID> s;
    for ( NodeID neighbor : neighbors[node] )
        s.insert(getEdgeLabel(node, neighbor));
    return std::vector<AgreeSetID>(s.begin(), s.end());
}

std::vector<AgreeSetID> InformativeGraph::getCertainAgreeSets(NodeID node) const
{
    std::unordered_set<AgreeSetID> s;
    for ( NodeID neighbor : neighbors[node] )
        if ( picked(neighbor) )
            s.insert(getEdgeLabel(node, neighbor));
    return std::vector<AgreeSetID>(s.begin(), s.end());
}

std::ostream& operator<<(std::ostream &os, const InformativeGraph &g)
{
    os << "neighbors:" << std::endl;
    for ( NodeID node = 0; node < g.nodeCount(); node++ )
        os << node << ": " << g.neighbors[node] << std::endl;
    os << "edgeLabels:" << std::endl;
    for ( std::pair<std::pair<NodeID,NodeID>, AgreeSetID> keyValue : g.edgeLabels )
        os << keyValue.first << " -> " << keyValue.second << std::endl;
    os << "picked:";
    for ( NodeID node = 0; node < g.nodeCount(); node++ )
        if ( g.picked(node) )
            os << ' ' << node;
    return os << std::endl;
}
