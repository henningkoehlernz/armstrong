#ifndef AGREE_SET_GRAPH_H
#define AGREE_SET_GRAPH_H

#include <vector>
#include <map>
#include <boost/dynamic_bitset.hpp>

typedef std::vector<size_t> Row;
typedef std::vector<Row> Table;
typedef boost::dynamic_bitset<> AttSet;
typedef std::vector<size_t> IndexSet;

IndexSet indexSetOf(const AttSet &x);

class ClosureCalculator
{
    const Table &table;
    const size_t columnCount;
    std::map<size_t, AttSet> memo;

    static size_t subHash(const Row &row, const IndexSet &x);
public:
    ClosureCalculator(const Table &table, size_t columnCount = 0);
    AttSet operator()(const AttSet &x);
    size_t columns() const;
};

std::vector<AttSet> getGenerators(ClosureCalculator &closure);

#endif
