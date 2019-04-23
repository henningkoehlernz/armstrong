#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/program_options.hpp>

#include "AgreeSetGraph.h"
#include "VectorUtil.h"
#include "BoostUtil.h"

using namespace std;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    size_t max_agree_set = 0;
    unsigned int max_backtrack = UINT_MAX;

    // extract command-line arguments
    try {
        po::options_description desc("Options");
        desc.add_options()
            ("help,h", "show options (this)")
            ("ag,a", po::value<size_t>(), "set limit on agree-sets")
            ("bt,b", po::value<unsigned int>(), "set limit on backtracking steps")
        ;
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            cout << desc << endl;
            return 0;
        }
        if ( vm.count("ag") )
            max_agree_set = vm["ag"].as<size_t>();
        if ( vm.count("bt") )
            max_backtrack = vm["bt"].as<unsigned int>();
    }
    catch(exception& e) {
        cerr << e.what() << "\n";
        return 1;
    }

    // init logging
    //boost::log::core::get()->set_filter( boost::log::trivial::severity >= boost::log::trivial::info );
    boost::log::core::get()->set_filter( boost::log::trivial::severity >= boost::log::trivial::debug );
    boost::log::add_common_attributes();
    boost::log::add_console_log(std::cout, boost::log::keywords::format = "[%TimeStamp%] %Message%");

    // parse agree-sets
    vector<AttributeSet> agreeSets;
    size_t line_counter = 0;
    while ( true )
    {
        AttributeSet agreeSet;
        if ( cin >> agreeSet )
        {
            line_counter++;
            boost::reverse(agreeSet); // bits are stored in left-to-right order
            if ( !agreeSet.none() && !agreeSet.all() && !contains(agreeSets, agreeSet) )
            {
                agreeSets.push_back(agreeSet);
                BOOST_LOG_TRIVIAL(debug) << line_counter << ": read " << agreeSet;
            }
            else
                BOOST_LOG_TRIVIAL(warning) << line_counter << ": skipping " << agreeSet;
        }
        else
            break;
    }
    if ( max_agree_set && agreeSets.size() > max_agree_set )
    {
        BOOST_LOG_TRIVIAL(info) << "finding Armstrong table for " << max_agree_set << "/" << agreeSets.size() << " agree-sets";
        agreeSets.resize(max_agree_set);
    }
    else
        BOOST_LOG_TRIVIAL(info) << "finding Armstrong table for " << agreeSets.size() << " agree-sets";
    // find armstrong table
    AgreeSetGraph g = findMinAgreeSetGraph(agreeSets, max_backtrack);
    //cout << g << endl;
    for ( vector<int> row : g.toArmstrongTable() )
        cout << row << endl;
    return 0;
}
