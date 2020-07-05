#include <boost/functional/hash.hpp>
#include <boost/tokenizer.hpp>
#include <iostream>

#include "CSVUtil.h"
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
