// global initialization - runs once before tests
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
struct InitLog {
    InitLog()
    {
        boost::log::core::get()->set_filter( boost::log::trivial::severity >= boost::log::trivial::warning );
    }
};
BOOST_GLOBAL_FIXTURE( InitLog );
