#ifndef AGREE_SET_GRAPH_H
#define AGREE_SET_GRAPH_H

#include <bits/stdc++.h>
#include <boost/dynamic_bitset.hpp>
using namespace std;

typedef vector<size_t> Row;
typedef vector<Row> Table;
typedef boost::dynamic_bitset<> AttSet;
typedef vector<size_t> IndexSet;

IndexSet indexSetOf(const AttSet &x);

class ClosureCalculator
{
    const Table &table;
    const size_t columnCount;
    map<size_t, AttSet> memo;

    static size_t subHash(const Row &row, const IndexSet &x);
public:
    ClosureCalculator(const Table &table, size_t columnCount = 0);
    AttSet operator()(const AttSet &x);
    size_t columns() const;
};

vector<AttSet> getGenerators(ClosureCalculator &closure);

#endif
