#ifndef AGREE_SET_EDGE_MINER_H
#define AGREE_SET_EDGE_MINER_H

#include <utilities>
#include "AgreeSetUtil.h"

std::vector<std::pair<size_t, size_t>> getAgreeSetEdges(const Table &table, const AttributeSet &agreeSet);

#endif
