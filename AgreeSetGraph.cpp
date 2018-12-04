#include <boost/log/trivial.hpp>
#include <boost/format.hpp>
#include <unordered_set>

#include "AgreeSetGraph.h"
#include "VectorUtil.h"

using namespace std;

//----------------- Util ------------------------

bool operator<=(const AttributeSet &a, const AttributeSet &b)
{
    return (a & ~b).none();
}

vector<AttID> diff(const AttributeSet &a, const AttributeSet &b)
{
    assert(a.size() == b.size());
    vector<AttID> d;
    for ( size_t i = 0; i < a.size(); i++ )
        if ( a[i] && !b[i] )
            d.push_back(i);
    return d;
}

//----------------- GenClosureOp ----------------

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
    AttributeSet closure(a.size());
    closure.flip();
    for ( const AttributeSet &gen : generators )
        if ( a <= gen )
            closure &= gen;
    return closure;
}

GenClosureOp::GenClosureOp(const vector<AttributeSet> &generators) : generators(generators) {}
GenClosureOp::GenClosureOp(const GenClosureOp &op) : generators(op.generators) {}

//----------------- EdgeData --------------------

AgreeSetGraph::EdgeData::EdgeData(size_t size) : attSet(size), assigned(false) {}
AgreeSetGraph::EdgeData::EdgeData(const EdgeData &e) : attSet(e.attSet), assigned(e.assigned) {}

ostream& operator<<(ostream &os, const AgreeSetGraph::EdgeData &edge)
{
    os << edge.attSet;
    if ( edge.assigned )
        os << '!';
    return os;
}

//----------------- AgreeSetGraph ---------------

EdgeID AgreeSetGraph::toEdge(NodeID a, NodeID b)
{
    assert(a != b);
    if ( a > b )
        return static_cast<EdgeID>(a) * (a - 1) / 2 + b;
    else
        return static_cast<EdgeID>(b) * (b - 1) / 2 + a;
}

void AgreeSetGraph::toNodes(EdgeID e, NodeID &a, NodeID &b)
{
    b = 0.5001 + sqrt(2.0*e + 0.25); // rounded down, increase slightly to avoid rounding errors
    a = e - (EdgeID)b * (b - 1) / 2;
}

void AgreeSetGraph::setIsoTwoPlus(NodeID node)
{
    switch ( isoType[node] )
    {
        case Connected::Pre:
            isoType[node] = isoType[node-1] = Connected::TwoPlus;
            break;
        case Connected::Post:
            isoType[node] = isoType[node+1] = Connected::TwoPlus;
            break;
        default:
            isoType[node] = Connected::TwoPlus;
            break;
    }
}

bool AgreeSetGraph::validate(string &msg) const
{
    // check that attComp and edges are consistent
    const size_t nc = nodeCount();
    const size_t ac = attributeCount();
    for ( AttID att = 0; att < ac; att++ )
        for ( NodeID a = 0; a < nc - 1; a++ )
            for ( NodeID b = a + 1; b < nc; b++ )
            {
                bool sameAttComp = (attComp[att][a] == attComp[att][b]);
                bool hasAttEdge = at(a,b)[att];
                if ( sameAttComp != hasAttEdge )
                {
                    msg = (boost::format("attComp!=edges for %1%@(%2%,%3%)") % att % a % b).str();
                    return false;
                }
            }
    return true;
}

AgreeSetGraph::AgreeSetGraph(size_t nodeCount, size_t attCount) : isoType(nodeCount, Connected::None)
{
    size_t edgeCount = nodeCount * (nodeCount - 1) / 2;
    edges.resize(edgeCount, EdgeData(attCount));
    // partitions all consist of single nodes
    Partition singleNodes(nodeCount);
    for ( size_t i = 0; i < nodeCount; i++ )
        singleNodes[i] = i;
    attComp.resize(attCount, singleNodes);
}

AgreeSetGraph::AgreeSetGraph(const AgreeSetGraph &g) : edges(g.edges), attComp(g.attComp), isoType(g.isoType) {}

size_t AgreeSetGraph::nodeCount() const
{
    return attComp.size() ? attComp[0].size() : 0;
}

size_t AgreeSetGraph::attributeCount() const
{
    return attComp.size();
}

const AttributeSet& AgreeSetGraph::at(NodeID a, NodeID b) const
{
    return edges[toEdge(a,b)].attSet;
}

bool AgreeSetGraph::canAssign(NodeID a, NodeID b, AttributeSet agreeSet) const
{
    const EdgeData &e = edges[toEdge(a, b)];
    if ( e.assigned || !(e.attSet <= agreeSet) )
        return false;
    // avoid isomorphic cases - only use smallest representative
    if ( isoType[a] == Connected::Pre || isoType[a] == Connected::Pre )
        return false;
    // special case of connecting two isolated nodes
    assert(a < b);
    if ( b > 1 && isoType[b-1] == Connected::None )
        return a == b-1 && isoType[b-2] != Connected::None;
    return true;
}

// Attribute + Location (=edge) it was added
struct AttLoc
{
    AttID att;
    NodeID a, b;
    AttLoc(AttID att, NodeID a, NodeID b) : att(att), a(a), b(b) {}
};

static ostream& operator<<(ostream &os, const AttLoc &attLoc)
{
    return os << (int)attLoc.att << "@(" << (int)attLoc.a << ',' << (int)attLoc.b << ')';
}

bool AgreeSetGraph::assign(NodeID a, NodeID b, AttributeSet agreeSet, const ClosureOp &closure)
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << "(" << (int)a << ',' << (int)b << ',' << agreeSet << ")";
    /*
    if ( !canAssign(a, b, agreeSet) )
        return false;
    */
    EdgeData &e = edges[toEdge(a, b)];
    // store attributes added to edges (causes near-cycles)
    vector<AttLoc> extraAtt;
    for ( AttID att : diff(agreeSet, e.attSet) )
        extraAtt.push_back(AttLoc(att, a, b));
    BOOST_LOG_TRIVIAL(trace) << "extraAtt = " << str(extraAtt) << " for g = " << *this << ", attComp = " << str(attComp);
    // set of all attributes, used later for pruning
    AttributeSet schema(attributeCount());
    schema.flip();
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
            const NodeID aVal = p[attLoc.a], bVal = p[attLoc.b];
            vector<NodeID> aComp, bComp;
            for ( NodeID i = 0; i < p.size(); i++ )
            {
                if ( p[i] == aVal )
                    aComp.push_back(i);
                else if ( p[i] == bVal )
                {
                    bComp.push_back(i);
                    p[i] = aVal; // merge partitions
                }
            }
            BOOST_LOG_TRIVIAL(trace) << "merging " << str(aComp) << " and " << str(bComp) << " for att=" << attLoc.att;
            // add att to all edges between components
            for ( NodeID aNode : aComp )
                for ( NodeID bNode : bComp )
                {
                    EdgeID eID = toEdge(aNode, bNode);
                    EdgeData &e = edges[eID];
                    if ( e.attSet[attLoc.att] )
                        continue;
                    if ( e.assigned )
                    {
                        BOOST_LOG_TRIVIAL(trace) << "failed at (" << (int)aNode << ',' << (int)bNode << ") += " << (int)attLoc.att << " for g = " << *this;
                        return false; // cannot extend assigned edges
                    }
                    e.attSet[attLoc.att] = true;
                    // extension may create non-closed set
                    openEdges.insert(eID);
                }
        }
        extraAtt.clear();
        {
            string msg;
            if ( !validate(msg) )
                BOOST_LOG_TRIVIAL(error) << msg << ", attComp = " << str(attComp) << ", g = " << *this;
        }
        // fix open edges
        for ( EdgeID eID : openEdges )
        {
            NodeID aNode, bNode;
            toNodes(eID, aNode, bNode);
            EdgeData &e = edges[eID];
            AttributeSet cl = closure(e.attSet);
            // closure shouldn't be entire attribute set, otherwise smaller solution exists
            if ( schema == cl )
            {
                BOOST_LOG_TRIVIAL(trace) << "assign(" << (int)a << ',' << (int)b << ',' << agreeSet << "): failed at ("
                    << (int)aNode << ',' << (int)bNode << ") = " << cl << " for g = " << *this;
                return false;
            }
            for ( AttID att : diff(cl, e.attSet) )
            {
                e.attSet[att] = true;
                extraAtt.push_back(AttLoc(att, aNode, bNode));
            }
        }
    }
    // update isoType to account for new edge
    if ( isoType[a] == Connected::None )
    {
        assert(b == a+1);
        assert(isoType[b] == Connected::None);
        isoType[a] = Connected::Post;
        isoType[b] = Connected::Pre;
    }
    else
    {
        setIsoTwoPlus(a);
        setIsoTwoPlus(b);
    }
    return true;
}

vector<vector<int>> AgreeSetGraph::toArmstrongTable() const
{
    const size_t nodeCount = this->nodeCount();
    const size_t attCount = attributeCount();
    vector<vector<int>> result(nodeCount, vector<int>(attCount));
    for ( size_t att = 0; att < attCount; att++ )
        for ( size_t node = 0; node < nodeCount; node++ )
            // bitset tracks attributes right to left
            result[node][att] = attComp[(attCount - 1) - att][node];
    return result;
}

ostream& operator<<(ostream &os, const AgreeSetGraph &g)
{
    os << "{ ";
    for ( EdgeID e = 0; e < g.edges.size(); e++ )
    {
        NodeID a, b;
        AgreeSetGraph::toNodes(e, a, b);
        os << '(' << (int)a << ',' << (int)b << "):" << g.edges[e] << ' ';
    }
    return os << '}';
}

//----------------- main function ---------------

class ProgressCounter
{
    size_t maxProgress;
public:
    void update(size_t progress)
    {
        while ( progress > maxProgress )
            cerr << (++maxProgress % 5 ? "." : to_string(maxProgress));
    }
    void reset()
    {
        if ( maxProgress )
        {
            maxProgress = 0;
            cerr << endl;
        }
    }
    ProgressCounter() : maxProgress(0) {}
    ~ProgressCounter() { reset(); }
};

AgreeSetGraph findMinAgreeSetGraph(const vector<AttributeSet> &agreeSets)
{
    // reduce to generators
    const vector<AttributeSet> generators = GenClosureOp::getGenerators(agreeSets);
    BOOST_LOG_TRIVIAL(debug) << "agreeSets = " << str(agreeSets);
    BOOST_LOG_TRIVIAL(debug) << "generators = " << str(generators);
    const GenClosureOp closure(generators);
    // find initial graph parameters
    const size_t attCount = generators.empty() ? 0 : generators[0].size();
    size_t nodeCount = 0.5001 + sqrt(2*generators.size() + 0.25);
    ProgressCounter progress;
    // main function (uses recursive backtracking, cannot use auto due to recursion)
    const function<bool(AgreeSetGraph&,const vector<AttributeSet>&,int)> extendGraph =
    [&extendGraph,&closure,&nodeCount,&progress](AgreeSetGraph &g, const vector<AttributeSet> &gen, size_t next = 0) -> bool
    {
        progress.update(next);
        //BOOST_LOG_TRIVIAL(trace) << "extendGraph(" << next << "): g = " << g;
        if ( next >= gen.size() )
            return true;
        for ( NodeID b = 1; b < nodeCount; b++ )
        {
            for ( NodeID a = 0; a < b; a++ )
                // quick & easy test before we take a copy
                if ( g.canAssign(a, b, gen[next]) )
                {
                    // work on copy to ensure g isn't modified if extension fails
                    AgreeSetGraph gPrime(g);
                    if ( gPrime.assign(a, b, gen[next], closure) && extendGraph(gPrime, gen, next + 1) )
                    {
                        g = gPrime;
                        return true;
                    }
                }
                else
                    BOOST_LOG_TRIVIAL(trace) << "extendGraph(" << next << "): cannot assign to (" << (int)a << ',' << (int)b << ") for g = " << g;
        }
        return false;
    };
    while ( nodeCount <= MAX_NODE )
    {
        BOOST_LOG_TRIVIAL(info) << "NodeCount = " << nodeCount;
        AgreeSetGraph g(nodeCount, attCount);
        // first generator can always be assigned
        if ( generators.size() > 0 )
            g.assign(0, 1, generators[0], closure);
        if ( extendGraph(g, generators, 1) )
            return g;
        // failure - try with more nodes
        nodeCount++;
        progress.reset();
    }
    exit(1);
}
