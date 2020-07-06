#ifndef AGREE_SET_EDGE_MINER_H
#define AGREE_SET_EDGE_MINER_H

#include "AgreeSetUtil.h"

// returns all vertex pairs with matching agree set
// first vertex < second vertex is guaranteed for all pairs
std::vector<std::pair<size_t, size_t>> getAgreeSetEdges(const Table &table, const AttributeSet &agreeSet);

#endif
