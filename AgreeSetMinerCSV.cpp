#include <boost/functional/hash.hpp>
#include <boost/tokenizer.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <iostream>
#include "AgreeSetMiner.h"
#include "VectorUtil.h"
#include "BoostUtil.h"

using namespace std;

void read_csv(Table &t, istream &in)
{
    string line;
    boost::hash<string> sHash;
    while ( getline(in, line) )
    {
        boost::tokenizer<boost::escaped_list_separator<char>> tok(line);
        Row row;
        for ( const string &s : tok )
            row.push_back(sHash(s));
        t.push_back(row);
    }
}

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
    // print to stdout
    sort(generators.begin(), generators.end());
    for ( AttributeSet &s : generators )
    {
        boost::reverse(s); // print bits in left-to-right order
        cout << s << endl;
    }
}
