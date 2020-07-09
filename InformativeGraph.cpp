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
    for ( std::pair<Edge, AgreeSetID> keyValue : edgeLabels )
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
    Edge edge = ordered(v,w);
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

std::vector<NodeID> InformativeGraph::getForced() const
{
    // organize edges by agree set and compute intersections
    static const std::vector<NodeID> NotVisited = { 999999999, 999999999 };
    std::vector<std::vector<NodeID>> forcedByAgreeSet;
    for ( std::pair<Edge,AgreeSetID> keyValue : edgeLabels )
    {
        // resize vector if needed
        if ( keyValue.second >= forcedByAgreeSet.size() )
            forcedByAgreeSet.resize(keyValue.second + 1, NotVisited);
        // intersect with new edge
        std::vector<NodeID> &forced = forcedByAgreeSet[keyValue.second];
        const Edge &e = keyValue.first;
        if ( forced == NotVisited )
        {
            forced[0] = e.first;
            forced[1] = e.second;
        }
        else if ( forced.size() == 2 )
        {
            // can have at most one overlap
            if ( forced[0] == e.first || forced[0] == e.second )
                forced.resize(1);
            else if ( forced[1] == e.first || forced[1] == e.second )
            {
                forced[0] = forced[1];
                forced.resize(1);
            }
            else
                forced.resize(0);
        }
        else if ( forced.size() == 1 )
        {
            if ( forced[0] != e.first && forced[0] != e.second )
                forced.resize(0);
        }
    }
    // combine into single vector
    std::unordered_set<NodeID> forcedNodes;
    for ( const std::vector<NodeID> &forced : forcedByAgreeSet )
        if ( forced != NotVisited )
            forcedNodes.insert(forced.begin(), forced.end());
    return std::vector<NodeID>(forcedNodes.begin(), forcedNodes.end());
}

std::vector<NodeID> InformativeGraph::getPicked() const
{
    std::vector<NodeID> result;
    for ( NodeID node = 0; node < nodeCount(); node++ )
        if ( picked(node) )
            result.push_back(node);
    return result;
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
    for ( NodeID node : g.getPicked() )
        os << ' ' << node;
    return os << std::endl;
}
