#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "AgreeSetGraph.h"
#include "VectorUtil.h"

using namespace std;

int main()
{
    // init logging
    //boost::log::core::get()->set_filter( boost::log::trivial::severity >= boost::log::trivial::info );
    boost::log::core::get()->set_filter( boost::log::trivial::severity >= boost::log::trivial::debug );
    // parse agree-sets
    vector<AttributeSet> agreeSets;
    size_t line_counter = 0;
    while ( true )
    {
        AttributeSet agreeSet;
        if ( cin >> agreeSet )
        {
            line_counter++;
            if ( !agreeSet.none() && !agreeSet.all() && !contains(agreeSets, agreeSet) )
            {
                agreeSets.push_back(agreeSet);
                BOOST_LOG_TRIVIAL(debug) << line_counter << ": read " << agreeSet;
            }
            else
                BOOST_LOG_TRIVIAL(warning) << line_counter << ": skipping " << agreeSet;
        }
        else
            break;
    }
    BOOST_LOG_TRIVIAL(info) << "finding Armstrong table for " << agreeSets.size() << " agree-sets";
    // find armstrong table
    AgreeSetGraph g = findMinAgreeSetGraph(agreeSets);
    cout << g << endl;
    for ( vector<int> row : g.toArmstrongTable() )
        cout << row << endl;
    return 0;
}
