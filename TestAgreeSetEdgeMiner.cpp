#define BOOST_TEST_MODULE TestAgreeSetMiner
#include <boost/test/unit_test.hpp>

#include "VectorUtil.h"
#include "AgreeSetEdgeMiner.h"

using namespace std;

#define AS(x) AttributeSet(string(#x))

static Table table = {
    { 0, 0, 0, 0 },
    { 0, 0, 1, 0 },
    { 1, 0, 0, 0 },
    { 2, 2, 2, 0 }
};

BOOST_AUTO_TEST_CASE( test_getAgreeSetEdges )
{
    vector<AttributeSet> gen = {
        AS(1011),
        AS(1110),
        AS(1000)
    };
    vector<vector<pair<size_t,size_t>>> expected = {
        { pair(0,1) },
        { pair(0,2) },
        { pair(0,3), pair(1,3), pair(2,3) }
    };
    for ( size_t test = 0; test < gen.size(); test++ )
    {
        vector<pair<size_t,size_t>> edges = getAgreeSetEdges(table, gen[test]);
        sort(edges.begin(), edges.end());
        BOOST_CHECK_EQUAL( edges, expected[test] );
    }
}
