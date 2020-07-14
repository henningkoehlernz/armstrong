#include <boost/log/trivial.hpp>
#include "DominanceGraph.h"

bool DominanceGraph::updateCertain(NodeID node)
{
    std::vector<AgreeSetID> c = getCertainAgreeSets(node);
    if ( c.empty() )
        return certain.erase(node);
    else
        return certain.insert(node, c);
}

bool DominanceGraph::updatePossible(NodeID node)
{
    std::vector<AgreeSetID> p = getPossibleAgreeSets(node);
    if ( p.empty() || picked(node) )
    {
        dominated.erase(node);
        return possible.erase(node);
    }
    else
    {
        dominated.insert(node);
        return possible.insert(node, p);
    }
}

NodeID DominanceGraph::findDominated()
{
    while ( !dominated.empty() )
    {
        NodeID node = *dominated.begin();
        // even if node used to be dominated, it may not be any longer
        // e.g. removal of one of two nodes that dominate each other could cause this
        if ( isDominated(node) )
            return node;
        else
            dominated.erase(node);
    }
    return NaNode;
}

DominanceGraph::DominanceGraph(const InformativeGraph &g) : InformativeGraph(g)
{
    for ( NodeID node = 0; node < nodeCount(); node++ )
    {
        updateCertain(node);
        updatePossible(node);
    }
    // dominance checks require possible & certain to be up-to-date
    for ( NodeID node = 0; node < nodeCount(); node++ )
        if ( certain.contains(node) )
            for ( NodeID dominatedNode : getDominated(node) )
                dominated.insert(dominatedNode);
}

void DominanceGraph::pickNode(NodeID node, std::unordered_set<NodeID> *updated)
{
    // ensure we are tracking updated nodes, these may become dominated
    std::unordered_set<NodeID> fallback;
    if ( updated == nullptr )
        updated = &fallback;
    InformativeGraph::pickNode(node, updated);
    // update certain & possible agree sets
    for ( NodeID updatedNode : *updated )
    {
        updateCertain(updatedNode);
        updatePossible(updatedNode);
    }
    // picked nodes no longer participate in dominance checks
    // no need to update certain, as picked node cannot have certain agree sets
    updatePossible(node);
    // neighbors need to update certain agree sets
    for ( NodeID neighbor : getNeighbors(node) )
    {
        // avoid updating twice
        if ( updated->count(neighbor) == 0 )
            updateCertain(neighbor);
        // neighbors of newly picked node may dominate new nodes
        // possible and certain[neighbor] are up-to-date, so getDominated will be correct
        for ( NodeID n : getDominated(neighbor) )
            dominated.insert(n);
    }
}

void DominanceGraph::removeNode(NodeID node)
{
    // neighbors need to update their possible agree sets
    std::vector<NodeID> oldNeighbors = getNeighbors(node);
    InformativeGraph::removeNode(node);
    for ( NodeID neighbor : oldNeighbors )
        updatePossible(neighbor);
    // remove node from dominance considerations
    updatePossible(node);
    updateCertain(node);
}

bool DominanceGraph::isDominated(NodeID node) const
{
    if ( degree(node) == 0 || picked(node) )
        return false;
    std::vector<NodeID> dom = certain.findSupersets(possible.at(node));
    return !dom.empty() && !(dom.size() == 1 && dom[0] == node);
}

std::vector<NodeID> DominanceGraph::getDominated(NodeID node) const
{
    if ( !certain.contains(node) )
        return std::vector<NodeID>(0);
    std::vector<NodeID> result;
    for ( NodeID dominated : possible.findSubsets(certain.at(node)) )
        if ( !picked(dominated) && dominated != node )
            result.push_back(dominated);
    return result;
}

void DominanceGraph::removeAllDominated(std::unordered_set<NodeID> *updated)
{
    NodeID dominatedNode = findDominated();
    while ( dominatedNode != NaNode )
    {
        if ( updated != nullptr )
        {
            for ( NodeID neighbor : getNeighbors(dominatedNode) )
                updated->insert(neighbor);
            updated->insert(dominatedNode);
        }
        removeNode(dominatedNode);
        dominatedNode = findDominated();
    }
}

std::vector<NodeID> DominanceGraph::prune(std::unordered_set<NodeID> *updated)
{
    std::unordered_set<NodeID> localUpdated;
    std::vector<NodeID> forcedNodes;
    auto propagateUpdates = [updated,&localUpdated]()
    {
        if ( updated != nullptr )
            updated->insert(localUpdated.begin(), localUpdated.end());
        localUpdated.clear();
    };
    do {
        propagateUpdates();
        // removing nodes can make nodes newly forced
        std::vector<NodeID> forced = getForced();
        BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << ": forced=" << forced;
        for ( NodeID node : forced )
            if ( !picked(node) )
            {
                pickNode(node, &localUpdated);
                propagateUpdates();
                forcedNodes.push_back(node);
            }
        // after picking forced nodes, we may get dominated nodes again
        removeAllDominated(&localUpdated);
        BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << ": localUpdated=" << localUpdated;
    } while ( !localUpdated.empty() );
    return forcedNodes;
}
