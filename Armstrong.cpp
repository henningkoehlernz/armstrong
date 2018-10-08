#include "Armstrong.h"

static int getMaxAtt(const vector<AttributeSet> &attSets)
{
    AttributeSet setUnion;
    for ( const AttributeSet &s : attSets )
        setUnion |= s;
    for ( int i = MAX_ATT - 1; i >= 0; i-- )
        if ( s[i] )
            return i;
    return -1;
}

AgreeSetGraph findMinAgreeSetGraph(const vector<AttributeSet> &agreeSets)
{
    // reduce to generators
    const vector<AttributeSet> generators = GenClosureOp::getGenerators(agreeSets);
    const GenClosureOp closure(generators);
    // find initial graph parameters
    const size_t attCount = getMaxAtt(generators) + 1;
    size_t nodeCount = 0.5001 + sqrt(2*generators.size() + 0.25);
    // main function (uses recursive backtracking)
    auto extendGraph = [&closure](AgreeSetGraph &g, const vector<AttributeSet> &gen, int next = 0) -> bool
    {
        if ( nextSet >= gen.size() )
            return true;
        for ( NodeId a = 0; a < nodeCount - 1; a++ )
            for ( NodeId b = a + 1; b < nodeCount; b++ )
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
        return false;
    };
    while (true)
    {
        AgreeSetGraph g(nodeCount, attCount);
        // first generator can always be assigned
        if ( generators.size() > 0 )
            g.assign(0, 1, generators[0], closure);
        if ( extendGraph(g, generators, 1) )
            return g;
        // failure - try with more nodes
        nodeCount++;
    }
}

vector<vector<int>> createArmstrongTable(const AgreeSetGraph &g)
{
}
