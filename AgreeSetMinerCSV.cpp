#include <boost/functional/hash.hpp>
#include <boost/tokenizer.hpp>
#include "AgreeSetMiner.h"

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
    // read table from stdin
    Table table;
    read_csv(table, cin);
    // find generating agree-sets
    ClosureCalculator closure(table);
    vector<AttSet> generators = getGenerators(closure);
    // print to stdout
    for ( const AttSet &s : generators )
        cout << s << endl;
}
