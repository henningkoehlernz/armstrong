#ifndef AGREE_SET_UTIL_H
#define AGREE_SET_UTIL_H

#include "AgreeSetTypes.h"

typedef std::vector<size_t> IndexSet;

IndexSet indexSetOf(const AttributeSet &x);
size_t subHash(const Row &row, const IndexSet &x);

#endif
