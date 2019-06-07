#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/program_options.hpp>
#include <cstdlib>

#include "AgreeSetMiner.h"
#include "AgreeSetGraph.h"
#include "VectorUtil.h"
#include "BoostUtil.h"

using namespace std;
namespace po = boost::program_options;

Table getRandomTable(size_t columns, size_t rows, size_t domain = 2)
{
    assert(domain > 1);
    Table table(rows, Row(columns));
    for ( Row &row : table )
        for ( size_t &value : row )
            value = rand() % domain;
    return table;
}

int main(int argc, char* argv[])
{
    size_t rows = 10, columns = 20;
    unsigned int max_backtrack = UINT_MAX;
    bool show_debug = false, show_trace = false;

    // initialize random seed
    srand(time(NULL));

    // extract command-line arguments
    try {
        po::options_description desc("Options");
        desc.add_options()
            ("help,h", "show options (this)")
            ("debug,d", "print debug information")
            ("trace,t", "print trace information (including debug)")
            ("bt,b", po::value<unsigned int>(), "set limit on backtracking steps")
            ("rows,r", po::value<size_t>(), "set number of rows")
            ("columns,c", po::value<size_t>(), "set number of columns")
        ;
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if ( vm.count("help") )
        {
            cout << desc << endl;
            return 0;
        }
        if ( vm.count("debug") )
            show_debug = true;
        if ( vm.count("trace") )
            show_trace = true;
        if ( vm.count("bt") )
            max_backtrack = vm["bt"].as<unsigned int>();
        if ( vm.count("rows") )
            rows = vm["rows"].as<size_t>();
        if ( vm.count("columns") )
            columns = vm["columns"].as<size_t>();
    }
    catch(exception& e) {
        cerr << e.what() << "\n";
        return 1;
    }

    // init logging
    if ( show_trace )
        boost::log::core::get()->set_filter( boost::log::trivial::severity >= boost::log::trivial::trace );
    else if ( show_debug )
        boost::log::core::get()->set_filter( boost::log::trivial::severity >= boost::log::trivial::debug );
    else
        boost::log::core::get()->set_filter( boost::log::trivial::severity >= boost::log::trivial::info );
    boost::log::add_common_attributes();
    boost::log::add_console_log(std::cout,
        boost::log::keywords::format = "[%TimeStamp%] %Message%",
        boost::log::keywords::auto_flush = true
    );
    // create agree sets
    Table table = getRandomTable(columns, rows);
    ClosureCalculator closure(table);
    vector<AttributeSet> agreeSets = getGenerators(closure);
    BOOST_LOG_TRIVIAL(info) << "finding Armstrong table for " << agreeSets.size() << " agree-sets (mined from " << rows << " rows)";
    // find armstrong table
    AgreeSetGraph g = findMinAgreeSetGraph(agreeSets, max_backtrack);
    return 0;
}
