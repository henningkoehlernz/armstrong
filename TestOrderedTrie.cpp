#define BOOST_TEST_MODULE TestOrderedTrie
#include <boost/test/unit_test.hpp>
#include <algorithm>

#include "OrderedTrie.h"
#include "VectorUtil.h"

using namespace std;

const vector<vector<int>> trieSets = {
    { 1, 2, 4 },
    { 1, 2, 5 },
    { 1, 4 },
    { 2 },
    { 2, 3 },
    { 2, 4 }
};

BOOST_AUTO_TEST_CASE( test_findSubsets )
{
    vector<int> superSet = { 1, 2, 4 };
    OrderedTrie<int,int> trie;
    for ( size_t id = 0; id < trieSets.size(); id++ )
        trie.insert(id, trieSets[id]);
    vector<int> subSets = trie.findSubsets(superSet);
    sort(subSets.begin(), subSets.end());
    vector<int> expected = { 0, 2, 3, 5 };
    BOOST_CHECK_EQUAL( subSets, expected );
}

BOOST_AUTO_TEST_CASE( test_findSupersets )
{
    vector<int> subSet = { 2, 4 };
    OrderedTrie<int,int> trie;
    for ( size_t id = 0; id < trieSets.size(); id++ )
        trie.insert(id, trieSets[id]);
    vector<int> superSets = trie.findSupersets(subSet);
    sort(superSets.begin(), superSets.end());
    vector<int> expected = { 0, 5 };
    BOOST_CHECK_EQUAL( superSets, expected );
}
