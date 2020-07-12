#ifndef DOMINANCE_GRAPH_H
#define DOMINANCE_GRAPH_H

#include "InformativeGraph.h"

/**
 * extends InformativeGraph by tracking dominance information
 */
class DominanceGraph : public InformativeGraph
{
private:
    OrderedTrie<NodeID,AgreeSetID> certain, possible;

    bool updateCertain(NodeID node);
    bool updatePossible(NodeID node);

public:
    // only construct after all edge inserts have been completed
    DominanceGraph(const InformativeGraph &g);

    virtual void pickNode(NodeID node, std::unordered_set<NodeID> *updated = nullptr) override;
    virtual void removeNode(NodeID node) override;

    // checks if node (other than self) exists whose certain agree set subsumes the possible agree set of node
    bool isDominated(NodeID node) const;
    // returns all non-isolated unpicked nodes dominated by given node
    std::vector<NodeID> getDominated(NodeID node) const;
};

#endif
