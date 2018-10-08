#define BOOST_TEST_MODULE MyTest
#include <boost/test/unit_test.hpp>

#include "AgreeSetGraph.h"

BOOST_AUTO_TEST_CASE( test_decode_encode )
{
    NodeID a, b;
    for ( EdgeID e = 0; e < 100; e++ )
    {
        AgreeSetGraph::toNodes(e, a, b);
        BOOST_CHECK_EQUAL( AgreeSetGraph::toEdge(a, b), e );
    }
}
