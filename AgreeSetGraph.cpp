#include "AgreeSetGraph.h"

//----------------- Util ------------------------

bool subset(AttributeSet a, AttributeSet b)
{
    for ( int i = 0; i < a.size(); i++ )
        if ( a[i] && !b[i] )
            return false;
    return true;
}

vector<NodeID> diff(AttributeSet a, AttributeSet b)
{
    vector<NodeID> d;
    for ( int i = 0; i < a.size(); i++ )
        if ( a[i] && !b[i] )
            d.push_back(i);
    return d;
}

//----------------- EdgeData --------------------

AgreeSetGraph::EdgeData::EdgeData(size_t attCount) : attSet(attCount)
{
}

AgreeSetGraph::EdgeData::EdgeData(const EdgeData &e) : attSet(e.attSet), assigned(e.assigned)
{
}

//----------------- AgreeSetGraph ---------------

EdgeID AgreeSetGraph::toEdge(NodeID a, NodeID b)
{
    assert(a != b);
    if ( a > b )
        return static_cast<EdgeID>(a) * (a - 1) + b;
    else
        return static_cast<EdgeID>(b) * (b - 1) + a;
}

void AgreeSetGraph::toNodes(EdgeID e, NodeID &a, NodeID &b)
{
    b = 0.5001 + sqrt(2*e + 0.25); // rounded down, increase slightly to avoid rounding errors
    a = e - b * (b - 1) / 2;
}

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

const AttributeSet& AgreeSetGraph::at(NodeID a, NodeID b) const
{
    return edges[toEdge(a,b)].attSet;
}

bool AgreeSetGraph::assign(NodeID a, NodeID b, AttributeSet agreeSet)
{
    struct AttLoc // Attribute + Location
    {
        AttID att;
        NodeID a, b;
        AttLoc(AttID att, NodeID a, NodeID b) : att(att), a(a), b(b) {}
    };
    EdgeData &e = edges[toEdge(a, b)];
    if ( e.assigned || !subset(e.attSet, agreeSet) )
        return false;
    // store attributes added to edges (causes near-cycles)
    vector<AttLoc> extraAtt;
    for ( AttID att : diff(agreeSet, e.attSet) )
        extraAtt.push_back(AttLoc(att, a, b));
    // now we can assign
    e.attSet = agreeSet;
    e.assigned = true;
    while ( !extraAtt.empty() )
    {
        // store edges which may no longer have closed attribute sets
        unordered_set<EdgeID> openEdges;
        // fix near-cycles & update partitions
        for ( AttLoc attLoc : extraAtt )
        {
            Partition &p = attComp[attLoc.att];
            // may have joined components already
            if ( p[attLoc.a] == p[attLoc.b] )
                continue;
            // find components to join
            vector<NodeID> aComp, bComp;
            for ( NodeID i = 0; i < p.size(); i++ )
            {
                if ( p[i] == p[attLoc.a] )
                    aComp.push_back(i);
                else if ( p[i] == p[attLoc.b] )
                {
                    bComp.push_back(i);
                    p[attLoc.b] = p[attLoc.a]; // merge partitions
                }
            }
            // add att to all edges between components
            for ( NodeID aNode : aComp )
                for ( NodeID bNode : bComp )
                {
                    EdgeID eID = toEdge(aNode, bNode);
                    EdgeData &e = edges[eID];
                    if ( e.attSet[attLoc.att] )
                        continue;
                    if ( e.assigned )
                        return false; // cannot extend assigned edges
                    e.attSet[attLoc.att] = true;
                    // extension may create non-closed set
                    openEdges.insert(eID);
                }
        }
        extraAtt.clear();
        // fix open edges
        for ( EdgeID eID : openEdges )
        {
            NodeID aNode, bNode;
            toNodes(eID, aNode, bNode);
            EdgeData &e = edges[eID];
            AttributeSet cl = closure(e.attSet);
            // TODO (potential optimization): closure shoudn't be entire attribute set
            for ( AttID att : diff(cl, e.attSet) )
            {
                e.attSet[att] = true;
                extraAtt.push_back(AttLoc(att, aNode, bNode));
            }
        }
    }
    return true;
}
