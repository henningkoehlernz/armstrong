#ifndef AGREE_SET_MINER_H
#define AGREE_SET_MINER_H

#include <map>
#include "AgreeSetTypes.h"
#include "AgreeSetUtil.h"

class ClosureCalculator
{
    const Table &table;
    const size_t columnCount;
    std::map<size_t, AttributeSet> memo;

public:
    ClosureCalculator(const Table &table, size_t columnCount = 0);
    AttributeSet operator()(const AttributeSet &x);
    size_t columns() const;
};

std::vector<AttributeSet> getGenerators(ClosureCalculator &closure);

#endif
