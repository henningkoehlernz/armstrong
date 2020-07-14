#define BOOST_TEST_MODULE TestInformativeGraph
#include <boost/test/unit_test.hpp>

#include "VectorUtil.h"
#include "DominanceGraph.h"
#include "BoostTestNoLog.h" // disable logging during test

using namespace std;

static const vector<vector<NodeID>> edges = {
    {0, 1, 0},
    {1, 2, 1},
    {2, 3, 1},
    {3, 4, 0},
    {4, 5, 2},
};

InformativeGraph toGraph()
{
    InformativeGraph g;
    for ( const vector<NodeID> &edge : edges )
        g.addEdge(edge[0], edge[1], edge[2]);
    return g;
}

BOOST_AUTO_TEST_CASE( test_getNeighbors )
{
    InformativeGraph g = toGraph();
    for ( const vector<NodeID> &edge : edges )
    {
        BOOST_CHECK(contains(g.getNeighbors(edge[0]), edge[1]));
        BOOST_CHECK(contains(g.getNeighbors(edge[1]), edge[0]));
    }
}

BOOST_AUTO_TEST_CASE( test_getForced )
{
    InformativeGraph g = toGraph();
    BOOST_CHECK_EQUAL(sorted(g.getForced()), vector<NodeID>({2, 4, 5}));
    // check again after removing node
    g.removeNode(0);
    BOOST_CHECK_EQUAL(sorted(g.getForced()), vector<NodeID>({2, 3, 4, 5}));
}

BOOST_AUTO_TEST_CASE( test_pickNode )
{
    InformativeGraph g = toGraph();
    std::unordered_set<NodeID> updated;
    BOOST_CHECK(!g.picked(4));
    g.pickNode(4, &updated);
    BOOST_CHECK(g.picked(4));
    BOOST_CHECK(updated.empty());
    std::unordered_set<NodeID> expected({0, 1, 3, 4});
    g.pickNode(3, &updated);
    BOOST_CHECK(g.picked(3));
    BOOST_CHECK_EQUAL(updated, expected);
}

BOOST_AUTO_TEST_CASE( test_getCertainAgreeSets )
{
    InformativeGraph g = toGraph();
    BOOST_CHECK(g.getCertainAgreeSets(3).empty());
    g.pickNode(2);
    BOOST_CHECK_EQUAL(g.getCertainAgreeSets(3), vector<NodeID>({1}));
    g.pickNode(4);
    BOOST_CHECK_EQUAL(g.getCertainAgreeSets(3), vector<NodeID>({0, 1}));
    g.pickNode(1);
    BOOST_CHECK_EQUAL(g.getCertainAgreeSets(3), vector<NodeID>({0}));
}

BOOST_AUTO_TEST_CASE( test_getPossibleAgreeSets )
{
    InformativeGraph g = toGraph();
    BOOST_CHECK_EQUAL(g.getPossibleAgreeSets(2), vector<NodeID>({1}));
    BOOST_CHECK_EQUAL(g.getPossibleAgreeSets(3), vector<NodeID>({0, 1}));
    g.pickNode(2);
    BOOST_CHECK_EQUAL(g.getPossibleAgreeSets(3), vector<NodeID>({0, 1}));
    g.pickNode(1);
    BOOST_CHECK_EQUAL(g.getPossibleAgreeSets(3), vector<NodeID>({0}));
}

//----------------- DominanceGraph --------------

BOOST_AUTO_TEST_CASE( test_isDominated )
{
    DominanceGraph g(toGraph());
    for ( NodeID node = 0; node < g.nodeCount(); node++ )
        BOOST_CHECK(!g.isDominated(node));
    g.pickNode(4);
    BOOST_CHECK(g.isDominated(0));
    BOOST_CHECK(!g.isDominated(5));
}

BOOST_AUTO_TEST_CASE( test_getDominated )
{
    DominanceGraph g(toGraph());
    for ( NodeID node = 0; node < g.nodeCount(); node++ )
        BOOST_CHECK(g.getDominated(node).empty());
    g.pickNode(4);
    BOOST_CHECK_EQUAL(g.getDominated(3), vector<NodeID>({0}));
    g.pickNode(2);
    BOOST_CHECK_EQUAL(g.getDominated(3), vector<NodeID>({0, 1}));
    BOOST_CHECK(g.getDominated(0).empty());
    g.pickNode(1);
    BOOST_CHECK_EQUAL(g.getDominated(0), vector<NodeID>({3}));
    BOOST_CHECK_EQUAL(g.getDominated(3), vector<NodeID>({0}));
}

BOOST_AUTO_TEST_CASE( test_removeAllDominated )
{
    DominanceGraph g(toGraph());
    g.pickNode(4);
    g.removeAllDominated();
    BOOST_CHECK(g.degree(0) == 0);
    BOOST_CHECK(g.degree(2) == 2);
    BOOST_CHECK(g.degree(4) == 2);
}

BOOST_AUTO_TEST_CASE( test_prune )
{
    DominanceGraph g(toGraph());
    std::vector<NodeID> picked = g.prune();
    std::sort(picked.begin(), picked.end());
    std::vector<NodeID> expected = { 2, 3, 4, 5 };
    BOOST_CHECK_EQUAL(picked, expected);
    BOOST_CHECK(g.degree(1) == 0);
}
