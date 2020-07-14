#ifndef DOMINANCE_GRAPH_H
#define DOMINANCE_GRAPH_H

#include "InformativeGraph.h"
#include "OrderedTrie.h"

/**
 * extends InformativeGraph by tracking dominance information
 */
class DominanceGraph : public InformativeGraph
{
private:
    static const NodeID NaNode = 999999999;

    // store certain and possible agree sets for efficient subset/superset checks
    OrderedTrie<NodeID,AgreeSetID> certain, possible;
    // all *potentially* dominated nodes
    std::unordered_set<NodeID> dominated;

    bool updateCertain(NodeID node);
    bool updatePossible(NodeID node);
    // returns NaNode if no dominated node exists
    NodeID findDominated();

public:
    // only construct after all edge inserts have been completed
    DominanceGraph(const InformativeGraph &g);

    virtual void pickNode(NodeID node, std::unordered_set<NodeID> *updated = nullptr) override;
    virtual void removeNode(NodeID node) override;

    /**
     * returns true if
     * (1) node is not isolated and not picked, and
     * (2) a different node exists whose certain agree set subsumes the possible agree set of node
     */
    bool isDominated(NodeID node) const;
    /**
     * returns all different nodes that are
     * (1) not isolated and not picked, and
     * (2) whose possible agree set is subsumed by the certain agree set of node
     */
    std::vector<NodeID> getDominated(NodeID node) const;
    /**
     * removes all dominated nodes, including nodes that become dominated in the process
     * if updated is set, inserts all nodes adjacent to edges removed
     */
    void removeAllDominated(std::unordered_set<NodeID> *updated = nullptr);
    /**
    * remove dominated nodes and pick forced nodes until neither of those nodes exists in the graph
    * inserts into updated all nodes adjacent to edges removed
    * returns nodes picked (due to being or becoming forced)
    */
    std::vector<NodeID> prune(std::unordered_set<NodeID> *updated = nullptr);
};

#endif
