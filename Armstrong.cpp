#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "AgreeSetGraph.h"
#include "VectorUtil.h"
#include "BoostUtil.h"

using namespace std;

int main(int argc, char* argv[])
{
    size_t max_agree_set = 0;
    if ( argc > 1 )
        max_agree_set = atoi(argv[1]);
    // init logging
    boost::log::core::get()->set_filter( boost::log::trivial::severity >= boost::log::trivial::info );
    //boost::log::core::get()->set_filter( boost::log::trivial::severity >= boost::log::trivial::debug );
    // parse agree-sets
    vector<AttributeSet> agreeSets;
    size_t line_counter = 0;
    while ( true )
    {
        AttributeSet agreeSet;
        if ( cin >> agreeSet )
        {
            line_counter++;
            boost::reverse(agreeSet); // bits are stored in left-to-right order
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
    if ( max_agree_set && agreeSets.size() > max_agree_set )
    {
        BOOST_LOG_TRIVIAL(info) << "finding Armstrong table for " << max_agree_set << "/" << agreeSets.size() << " agree-sets";
        agreeSets.resize(max_agree_set);
    }
    else
        BOOST_LOG_TRIVIAL(info) << "finding Armstrong table for " << agreeSets.size() << " agree-sets";
    // find armstrong table
    AgreeSetGraph g = findMinAgreeSetGraph(agreeSets);
    cout << g << endl;
    for ( vector<int> row : g.toArmstrongTable() )
        cout << row << endl;
    return 0;
}
