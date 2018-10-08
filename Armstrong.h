#ifndef ARMSTRONG_H
#define ARMSTRONG_H

#include "AgreeSetGraph.h"

AgreeSetGraph findMinAgreeSetGraph(const vector<AttributeSet> &agreeSets);
vector<vector<int>> createArmstrongTable(const AgreeSetGraph &g);

#endif
