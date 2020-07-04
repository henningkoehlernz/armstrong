#include <boost/log/trivial.hpp>
#include "AgreeSetEdgeMiner.h"

using namespace std;

std::vector<std::pair<size_t, size_t>> getAgreeSetEdges(const Table &table, const AttributeSet &agreeSet)
{
    // hash rows based on values in agreeSet & sort by hash
    struct RowRef
    {
        size_t rowID;
        size_t hashValue;
        bool operator<(const RowRef &other) const { return hashValue < other.hashValue; }
    };
    vector<RowRef> rowRefs(table.size());
    IndexSet columnSet = indexSetOf(agreeSet);
    for ( size_t i = 0; i < table.size(); i++ )
    {
        rowRefs[i].rowID = i;
        rowRefs[i].hashValue = subHash(table[i], columnSet);
    }
    sort(rowRefs.begin(), rowRefs.end());
    // identify vertex pairs with matching agree sets
    std::vector<std::pair<size_t, size_t>> result;
    for ( size_t v = 0; v+1 < table.size(); v++ )
        for ( size_t w = v+1; w < table.size() && rowRefs[v].hashValue == rowRefs[w].hashValue; w++ )
        {
            // check that agree set matches
            bool match = true;
            const Row &rowOfV = table[rowRefs[v].rowID];
            const Row &rowOfW = table[rowRefs[w].rowID];
            for ( size_t att = 0; att < rowOfV.size(); att++ )
                if ( (rowOfV[att] == rowOfW[att]) != agreeSet[att] )
                {
                    match = false;
                    break;
                }
            if ( match )
                result.push_back(std::make_pair(rowRefs[v].rowID, rowRefs[w].rowID));
        }
    return result;
}
