#include <boost/functional/hash.hpp>
#include <boost/tokenizer.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <iostream>

#include "CSVUtil.h"
#include "AgreeSetMiner.h"
#include "AgreeSetEdgeMiner.h"
#include "VectorUtil.h"
#include "BoostUtil.h"

using namespace std;

class LabeledEdge
{
public:
    size_t v, w, label;
    bool operator<(const LabeledEdge &other) {
#define compare(X,Y) if (X < Y) return true; if (X > Y) return false
        compare(v, other.v);
        compare(w, other.w);
#undef compare
        return label < other.label;
    }
    LabeledEdge(size_t v, size_t w, size_t label) : v(v), w(w), label(label) {}
};

int main(int argc, char *argv[])
{
    boost::log::core::get()->set_filter( boost::log::trivial::severity >= boost::log::trivial::warning );
    // read table from stdin
    Table table;
    read_csv(table, cin);
    BOOST_LOG_TRIVIAL(debug) << "table = " << table << endl;
    // find generating agree-sets
    ClosureCalculator closure(table);
    vector<AttributeSet> generators = getGenerators(closure);
    sort(generators.begin(), generators.end());
    // find corresponding agree-set edges
    vector<LabeledEdge> edges;
    for ( size_t genID = 0; genID < generators.size(); genID++ )
        for ( std::pair<size_t, size_t> &edge : getAgreeSetEdges(table, generators[genID]) )
            edges.push_back(LabeledEdge(edge.first, edge.second, genID));
    sort(edges.begin(), edges.end());
    // print to stdout
    for ( LabeledEdge &e : edges )
        cout << e.v << ' ' << e.w << ' ' << e.label << endl;
}
