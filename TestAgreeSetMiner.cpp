#define BOOST_TEST_MODULE TestAgreeSetMiner
#include <boost/test/unit_test.hpp>
#include "AgreeSetMiner.h"
#include "VectorUtil.h"
#include "BoostUtil.h"

static Table table = {
    { 0, 0, 0 },
    { 0, 0, 1 },
    { 1, 0, 0 },
    { 2, 2, 2 }
};
static ClosureCalculator closure(table);

BOOST_AUTO_TEST_CASE( test_ClosureCalculator )
{
    BOOST_CHECK_EQUAL( closure.columns(), table[0].size() );
#define ASP(x,y) { AttSet(string(#x)), AttSet(string(#y)) }
    map<AttSet,AttSet> setToClosure = {
        ASP(000,000),
        ASP(001,011),
        ASP(010,010),
        ASP(100,110)
    };
#undef ASP
    for ( auto const& mapping : setToClosure )
        BOOST_CHECK_EQUAL( closure(mapping.first), mapping.second );
}

BOOST_AUTO_TEST_CASE( test_getGenerators )
{
    vector<AttSet> gen = getGenerators(closure);
    BOOST_CHECK( contains(gen, AttSet(string("011"))) );
    BOOST_CHECK( contains(gen, AttSet(string("110"))) );
    BOOST_CHECK_EQUAL( gen.size(), 2 );
}
