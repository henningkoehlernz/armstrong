#include <boost/functional/hash.hpp>

#include "AgreeSetUtil.h"

using namespace std;

IndexSet indexSetOf(const AttributeSet &x)
{
    IndexSet result;
    for ( size_t i = 0; i < x.size(); ++i )
        if ( x[i] )
            result.push_back(i);
    return result;
}

size_t subHash(const Row &row, const IndexSet &x)
{
    size_t seed = 0;
    for ( size_t index : x )
        // see https://stackoverflow.com/questions/19966041/getting-too-many-collisions-with-hash-combine
        boost::hash_combine(seed, row[index] * 2654435761);
    return seed;
}
