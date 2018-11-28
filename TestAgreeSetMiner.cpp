#define BOOST_TEST_MODULE TestAgreeSetMiner
#include <boost/test/unit_test.hpp>
#include "AgreeSetMiner.h"

BOOST_AUTO_TEST_CASE( test_ClosureCalculator )
{
    Table table = {
        { 0, 0, 0 },
        { 0, 1, 1 }
    };
    AttSet x(string("001"));
    ClosureCalculator closure(table);
    BOOST_CHECK_EQUAL( closure.columns(), 3 );
    BOOST_CHECK_EQUAL( closure(x), x );
}
