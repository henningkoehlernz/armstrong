#ifndef AGREE_SET_TYPES_H
#define AGREE_SET_TYPES_H

#include <string>
#include <boost/dynamic_bitset.hpp>

typedef std::vector<size_t> Row;
typedef std::vector<Row> Table;
// attributes are encoded as integers 0..k
typedef boost::dynamic_bitset<> AttributeSet;

#endif
