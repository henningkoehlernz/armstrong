#include "DominanceGraph.h"

bool DominanceGraph::updateCertain(NodeID node)
{
    vector<AgreeSetID> c = g.getCertainAgreeSets(node);
    if ( c.empty() )
        return certain.remove(node);
    else
        return certain.insert(node, c);
}

bool DominanceGraph::updatePossible(NodeID node)
{
    vector<AgreeSetID> p = g.getPossibleAgreeSets(node);
    if ( p.empty() )
        return possible.remove(node);
    else
        return possible.insert(node, p);
}

DominanceGraph::DominanceGraph(const InformativeGraph &g) : InformativeGraph(g)
{
    for ( NodeID node = 0; node < nodeCount(); node++ )
    {
        updateCertain(node);
        updatePossible(node);
    }
}

void DominanceGraph::pickNode(NodeID node, std::unordered_set<NodeID> *updated)
{
    // ensure we are tracking updated nodes, these may become dominated
    std::unordered_set<NodeID> fallback;
    if ( updated == nullptr )
        updated = &fallback;
    InformativeGraph::pickNode(node, updated);
    // update certain & possible agree sets
    for ( NodeID updatedNode : updated )
    {
        updateCertain(updatedNode);
        updatePossible(updatedNode);
    }
    // neighbors need to update certain agree sets
    for ( NodeID neighbor : getNeighbors(node) )
        // avoid updating twice
        if ( updated.count(neighbor) == 0 )
            updateCertain(neighbor);
}

void DominanceGraph::removeNode(NodeID node)
{
    // neighbors need to update their possible agree sets
    std::vector<NodeID> oldNeighbors = getNeighbors(node);
    InformativeGraph::removeNode(node);
    for ( NodeID neighbor : oldNeighbors )
    {
        std::vector<AgreeSetID> p = g.getPossibleAgreeSets(neighbor);
        if ( p.empty() )
            possible.remove(updatedNode);
        else
            possible.insert(updatedNode, p);
    }
}

bool DominanceGraph::isDominated(NodeID node) const
{
    if ( degree(node) == 0 )
        return nodeCount() > 1;
    std::vector<NodeID> dom = certain.findSuperSets(possible.at(node));
    return !dom.empty() && !(dom.size() == 1 && dom[0] == node);
}

std::vector<NodeID> DominanceGraph::getDominated(NodeID node) const
{
    if ( !certain.contains(node) )
        return std::vector<NodeID>(0);
    std::vector<NodeID> result;
    for ( NodeID dominated : possible.findSubsets(certain.at(node) )
        if ( !picked(dominated) && dominated != node )
            result.push_back(dominated);
    return result;
}
