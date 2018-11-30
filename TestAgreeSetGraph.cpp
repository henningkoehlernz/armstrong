#define BOOST_TEST_MODULE TestAgreeSetGraph
#include <boost/test/unit_test.hpp>

#include "VectorUtil.h"
#include "AgreeSetGraph.h"

using namespace std;

#define AS(x) AttributeSet(string(#x))

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
    const vector<AttributeSet> agreeSets = { AS(1110), AS(0110), AS(0111) };
    const vector<AttributeSet> generators = { AS(1110), AS(0111) };
    BOOST_CHECK_EQUAL( str(GenClosureOp::getGenerators(agreeSets)), str(generators) );
}

BOOST_AUTO_TEST_CASE( test_AttributeSetCompare )
{
    BOOST_CHECK( AS(0101) <= AS(0111) );
    BOOST_CHECK( AS(0101) <= AS(0101) );
    BOOST_CHECK( !(AS(1011) <= AS(1101)) );
    BOOST_CHECK( !(AS(1101) <= AS(1011)) );
}

BOOST_AUTO_TEST_CASE( test_AttributeSetDiff )
{
    AttributeSet a(string("1011")), b(string("0010"));
    vector<AttID> expected = { 0, 3 };
    BOOST_CHECK_EQUAL( str(diff(a,b)), str(expected) );
}
