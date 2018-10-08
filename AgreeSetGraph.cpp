#include "AgreeSetGraph.h"

//----------------- Util ------------------------

bool operator<=(const AttributeSet &a, const AttributeSet &b)
{
    return (a & ~b).none();
}

static vector<NodeID> diff(const AttributeSet &a, const AttributeSet &b)
{
    vector<NodeID> d;
    for ( int i = 0; i < a.size(); i++ )
        if ( a[i] && !b[i] )
            d.push_back(i);
    return d;
}

//----------------- GenClosureOp ----------------

template <typename T>
static vector<T> operator-(const vector<T> &v, const T &value)
{
    vector<T> result;
    for ( const T &elem : v )
        if ( elem != value )
            result.push_back(elem);
    return result;
}

template <typename T>
static bool contains(const vector<T> &v, const T &value)
{
    for ( const T &elem : v )
        if ( elem == value )
            return true;
    return false;
}

vector<AttributeSet> GenClosureOp::getGenerators(const vector<AttributeSet> &agreeSets)
{
    vector<AttributeSet> result;
    for ( const AttributeSet &ag : agreeSets )
    {
        GenClosureOp genOp(agreeSets - ag);
        if ( genOp(ag) != ag )
            result.push_back(ag);
    }
    return result;
}

AttributeSet GenClosureOp::operator()(const AttributeSet &a) const
{
    AttributeSet closure;
    for ( const AttributeSet &gen : generators )
        if ( a <= gen )
            closure &= gen;
    return closure;
}

GenClosureOp::GenClosureOp(const vector<AttributeSet> &generators) : generators(generators) {}
GenClosureOp::GenClosureOp(const GenClosureOp &op) : generators(op.generators) {}

//----------------- EdgeData --------------------

AgreeSetGraph::EdgeData::EdgeData() {}
AgreeSetGraph::EdgeData::EdgeData(const EdgeData &e) : attSet(e.attSet), assigned(e.assigned) {}

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

AgreeSetGraph::AgreeSetGraph(size_t nodeCount, size_t attCount)
{
    size_t edgeCount = nodeCount * (nodeCount - 1) / 2;
    edges.resize(edgeCount);
    // partitions all consist of single nodes
    Partition singleNodes(nodeCount);
    for ( int i = 0; i < nodeCount; i++ )
        singleNodes[i] = i;
    attComp.resize(attCount, singleNodes);
}

AgreeSetGraph::AgreeSetGraph(const AgreeSetGraph &g) : edges(g.edges), attComp(g.attComp) {}

const AttributeSet& AgreeSetGraph::at(NodeID a, NodeID b) const
{
    return edges[toEdge(a,b)].attSet;
}

bool AgreeSetGraph::canAssign(NodeID a, NodeID b, AttributeSet agreeSet) const
{
    const EdgeData &e = edges[toEdge(a, b)];
    return !e.assigned && e.attSet <= agreeSet;
}

bool AgreeSetGraph::assign(NodeID a, NodeID b, AttributeSet agreeSet, const ClosureOp &closure)
{
    if ( !canAssign(a, b, agreeSet) )
        return false;
    // Attribute + Location (=edge) it was added
    struct AttLoc
    {
        AttID att;
        NodeID a, b;
        AttLoc(AttID att, NodeID a, NodeID b) : att(att), a(a), b(b) {}
    };
    EdgeData &e = edges[toEdge(a, b)];
    // store attributes added to edges (causes near-cycles)
    vector<AttLoc> extraAtt;
    for ( AttID att : diff(agreeSet, e.attSet) )
        extraAtt.push_back(AttLoc(att, a, b));
    // set of all attributes, used later for pruning
    AttributeSet schema;
    for ( size_t i = 0; i < attComp.size(); i++ )
        schema[i] = true;
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
            // closure shouldn't be entire attribute set, otherwise smaller solution exists
            if ( schema == cl )
                return false;
            for ( AttID att : diff(cl, e.attSet) )
            {
                e.attSet[att] = true;
                extraAtt.push_back(AttLoc(att, aNode, bNode));
            }
        }
    }
    return true;
}
