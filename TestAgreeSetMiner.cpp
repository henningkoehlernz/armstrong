#define BOOST_TEST_MODULE TestAgreeSetMiner
#include <boost/dynamic_bitset.hpp>
#include <boost/test/unit_test.hpp>
#include <map>

#include "VectorUtil.h"
#include "AgreeSetMiner.h"
#include "BoostUtil.h"
#include "BoostTestNoLog.h" // disable logging during test

using namespace std;

#define AS(x) AttributeSet(string(#x))
#define ASP(x,y) { AS(x), AS(y) }

static Table table = {
    { 0, 0, 0, 0 },
    { 0, 0, 1, 0 },
    { 1, 0, 0, 0 },
    { 2, 2, 2, 0 }
};
static ClosureCalculator closure(table);

BOOST_AUTO_TEST_CASE( test_ClosureCalculator )
{
    BOOST_CHECK_EQUAL( closure.columns(), table[0].size() );
    map<AttributeSet,AttributeSet> setToClosure = {
        ASP(0000,1000),
        ASP(0001,1011),
        ASP(0010,1010),
        ASP(0100,1110),
        ASP(1000,1000)
    };
    for ( auto const& mapping : setToClosure )
        BOOST_CHECK_EQUAL( closure(mapping.first), mapping.second );
}

BOOST_AUTO_TEST_CASE( test_getGenerators )
{
    vector<AttributeSet> gen = getGenerators(closure);
    vector<AttributeSet> expected = {
        AS(1011),
        AS(1110),
        AS(1000)
    };
    sort(gen.begin(), gen.end());
    sort(expected.begin(), expected.end());
    BOOST_CHECK_EQUAL( gen, expected );
}
