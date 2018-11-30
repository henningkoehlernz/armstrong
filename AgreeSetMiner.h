#ifndef AGREE_SET_GRAPH_H
#define AGREE_SET_GRAPH_H

#include <map>
#include "AgreeSetTypes.h"

typedef std::vector<size_t> IndexSet;

IndexSet indexSetOf(const AttributeSet &x);

class ClosureCalculator
{
    const Table &table;
    const size_t columnCount;
    std::map<size_t, AttributeSet> memo;

    static size_t subHash(const Row &row, const IndexSet &x);
public:
    ClosureCalculator(const Table &table, size_t columnCount = 0);
    AttributeSet operator()(const AttributeSet &x);
    size_t columns() const;
};

std::vector<AttributeSet> getGenerators(ClosureCalculator &closure);

#endif
