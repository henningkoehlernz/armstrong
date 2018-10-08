#define BOOST_TEST_MODULE TestAgreeSetGraph
#include <boost/test/unit_test.hpp>

#include "VectorUtil.h"
#include "AgreeSetGraph.h"

BOOST_AUTO_TEST_CASE( test_encode )
{
    BOOST_CHECK_EQUAL( AgreeSetGraph::toEdge(0, 1), 0 );
    BOOST_CHECK_EQUAL( AgreeSetGraph::toEdge(0, 2), 1 );
    BOOST_CHECK_EQUAL( AgreeSetGraph::toEdge(1, 2), 2 );
}

BOOST_AUTO_TEST_CASE( test_decode_encode )
{
    NodeID a, b;
    for ( EdgeID e = 0; e < 100; e++ )
    {
        AgreeSetGraph::toNodes(e, a, b);
        BOOST_CHECK_EQUAL( AgreeSetGraph::toEdge(a, b), e );
    }
}

BOOST_AUTO_TEST_CASE( test_getGenerators )
{
    const vector<AttributeSet> agreeSets = { AttributeSet("1110"), AttributeSet("0110"), AttributeSet("0111") };
    const vector<AttributeSet> generators = { AttributeSet("1110"), AttributeSet("0111") };
    BOOST_CHECK_EQUAL(str(GenClosureOp::getGenerators(agreeSets)), str(generators));
}
