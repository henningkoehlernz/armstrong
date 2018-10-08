#include "AgreeSetGraph.h"
#include "VectorUtil.h"

int main()
{
    vector<AttributeSet> agreeSets;
    while ( !cin.eof() )
    {
        AttributeSet agreeSet;
        cin >> agreeSet;
        if ( !agreeSet.none() && !agreeSet.all() && !contains(agreeSets, agreeSet) )
            agreeSets.push_back(agreeSet);
    }
    AgreeSetGraph g = findMinAgreeSetGraph(agreeSets);
    cout << g << endl;
    return 0;
}
