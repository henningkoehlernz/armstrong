#include "AgreeSetGraph.h"

//----------------- Util ------------------------

bool subset(AgreeSet a, AgreeSet b)
{
    for ( int i = 0; i < a.size(); i++ )
        if ( a[i] && !b[i] )
            return false;
    return true;
}

vector<NodeID> diff(AgreeSet a, AgreeSet b)
{
    vector<NodeID> d;
    for ( int i = 0; i < a.size(); i++ )
        if ( a[i] && !b[i] )
            d.push_back(i);
    return d;
}

//----------------- EdgeData --------------------

EdgeData::EdgeData(size_t attCount) : attSet(attCount)
{
}

EdgeData::EdgeData(const EdgeData &e) : attSet(e.attSet), assigned(e.assigned)
{
}

EdgeID AgreeSetGraph::edge(NodeID a, NodeID b)
{
    assert(a != b);
    if ( a > b )
        return static_cast<EdgeID>(a - 1) * (a - 2) + b;
    else
        return static_cast<EdgeID>(b - 1) * (b - 2) + a;
}

//----------------- AgreeSetGraph ---------------

AgreeSetGraph::AgreeSetGraph(size_t nodeCount, size_t attCount, const ClosureOp &closure) : closure(closure)
{
    size_t edgeCount = nodeCount * (nodeCount - 1) / 2;
    edges.resize(edgeCount, attCount);
    // partitions all consist of single nodes
    Partition singleNodes(nodeCount);
    for ( int i = 0; i < nodeCount; i++ )
        singleNodes[i] = i;
    attComp.resize(attCount, singleNodes);
}

AgreeSetGraph::AgreeSetGraph(const AgreeSetGraph &g) : closure(g.closure), edges(g.edges), attComp(g.attComp)
{
}

const AgreeSet& AgreeSetGraph::at(NodeID a, NodeID b) const
{
    return edges[edge(a,b)].attSet;
}

bool AgreeSetGraph::assign(NodeID a, NodeID b, AgreeSet value)
{
    EdgeData &e = edges[edge(a, b)];
    if ( e.assigned || !subset(e.attSet, value) )
        return false;
    // store attributes added to edge for later processing
    vector<AttID> extraAtt = diff(value, e.attSet);
    e.attSet = value;
    e.assigned = true;
    // fix near-cycles & update partitions
    unordered_set<EdgeID> openEdges;
    for ( AttID att : extraAtt )
    {
        Partition &p = attComp[att];
        // find components to join
        vector<NodeID> aComp, bComp;
        for ( NodeID i = 0; i < p.size(); i++ )
        {
            if ( p[i] == p[a] )
                aComp.push_back(i);
            else if ( p[i] == p[b] )
            {
                bComp.push_back(i);
                p[b] = p[a]; // merge partitions
            }
        }
        // add att to all edges between components
        for ( NodeID aNode : aComp )
            for ( NodeID bNode : bComp )
            {
                EdgeID eID = edge(aNode, bNode);
                EdgeData &e = edges[eID];
                if ( e.attSet[att] )
                    continue;
                if ( e.assigned )
                    return false; // cannot extend assigned edges
                e.attSet[att] = true;
                // extension may create non-closed set
                openEdges.insert(eID);
            }
    }
    return true;
}
