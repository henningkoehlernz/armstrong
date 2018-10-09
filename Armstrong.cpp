#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "AgreeSetGraph.h"
#include "VectorUtil.h"

int main()
{
    // init logging
    //boost::log::core::get()->set_filter( boost::log::trivial::severity >= boost::log::trivial::info );
    // parse agree-sets
    vector<AttributeSet> agreeSets;
    while ( !cin.eof() )
    {
        AttributeSet agreeSet;
        cin >> agreeSet;
        if ( !agreeSet.none() && !agreeSet.all() && !contains(agreeSets, agreeSet) )
            agreeSets.push_back(agreeSet);
    }
    // find armstrong table
    AgreeSetGraph g = findMinAgreeSetGraph(agreeSets);
    cout << g << endl;
    for ( vector<int> row : g.toArmstrongTable() )
        cout << row << endl;
    return 0;
}
