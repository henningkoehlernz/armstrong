#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/program_options.hpp>
#include <queue>

#include "DominanceGraph.h"

using namespace std;
namespace po = boost::program_options;

struct GreedyNode
{
    uint16_t certainSize, degree;
    NodeID node;

    GreedyNode(uint16_t certainSize, uint16_t degree, NodeID node) : certainSize(certainSize), degree(degree), node(node) {}
    GreedyNode(const InformativeGraph &g, NodeID node) : node(node)
    {
        degree = g.degree(node);
        certainSize = degree > 0 ? g.getCertainAgreeSets(node).size() : 0;
    }

    bool operator<(const GreedyNode &other) const
    {
#define compare(X,Y) if (X < Y) return true; if (X > Y) return false
        compare(certainSize, other.certainSize);
        compare(degree, other.degree);
#undef compare
        return node < other.node;
    }
    bool operator==(const GreedyNode &other) const
    {
        return certainSize == other.certainSize
            && degree == other.degree
            && node == other.node;
    }
};

/**
 * repeatedly pick node with largest (certain agree-set, degree) pair
 * optionally prunes dominated nodes between greedy steps
 * forced nodes are always picked first
 */
std::vector<NodeID> pickGreedy(const InformativeGraph &graph, bool pruning)
{
    std::priority_queue<GreedyNode> queue;
    const size_t nodeCount = graph.nodeCount();
    // only use DominanceGraph if we need to prune dominated nodes
    InformativeGraph *g = pruning ? new DominanceGraph(graph) : new InformativeGraph(graph);
    DominanceGraph *gDom = dynamic_cast<DominanceGraph*>(g); // to avoid repeated casting later
    // prune once to ensure we only pick greedily on pruned graph
    if ( pruning )
    {
        gDom->prune();
    }
    else
    {
        for ( NodeID node : g->getForced() )
        {
            BOOST_LOG_TRIVIAL(info) << "picking forced node " << node;
            g->pickNode(node);
        }
    }
    // prepare queue
    for ( NodeID node = 0; node < nodeCount; node++ )
    {
        if ( g->degree(node) > 0 )
            queue.push(GreedyNode(*g, node));
    }
    // repeated pick top element
    while ( !queue.empty() )
    {
        GreedyNode next = queue.top();
        queue.pop();
        // check that node data is still accurate
        GreedyNode currentValue(*g, next.node);
        if ( next == currentValue )
        {
            if ( !g->picked(next.node) )
            {
                BOOST_LOG_TRIVIAL(info) << "picking node " << next.node;
                // we don't care about updates from edge removal as this will only lower greedy weight
                g->pickNode(next.node);
                // certainSize of neighbors may have increased
                for ( NodeID neighbor : g->getNeighbors(next.node) )
                    // any current neighbor will have degree > 0
                    queue.push(GreedyNode(*g, neighbor));
                if ( pruning )
                {
                    // graph has changed, so we prune again - again ignoring edge removals
                    std::vector<NodeID> pickedWhilePruning = gDom->prune();
                    // neighbors of picked nodes need to be re-enqueued
                    for ( NodeID pickedNode : pickedWhilePruning )
                        for ( NodeID neighbor : g->getNeighbors(pickedNode) )
                            queue.push(GreedyNode(*g, neighbor));
                }
            }
        }
        else if ( currentValue < next && currentValue.degree > 0 )
        {
            // re-enqueue for later processing
            queue.push(currentValue);
        }
    }
    return g->getPicked();
}

void runGreedy(const InformativeGraph &g, bool pruning, bool show)
{
    vector<NodeID> result = pickGreedy(g, pruning);
    if ( show )
        cout << (pruning ? "Pruned: " : "Greedy: ") << result.size() << " nodes picked: " << result << endl;
    else
        cout << (pruning ? "Pruned: " : "Greedy: ") << result.size() << " nodes picked" << endl;
}

int main(int argc, char* argv[])
{
    bool showResult = false;
    bool debug = false;
    // extract command-line arguments
    try {
        po::options_description desc("Options");
        desc.add_options()
            ("help,h", "show options (this)")
            ("debug,d", "print debug information")
            ("show,s", "show solution")
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
            debug = true;
        if ( vm.count("show") )
            showResult = true;
    } catch(exception& e) {
        cerr << e.what() << "\n";
        return 1;
    }

    // init logging
    if ( debug )
        boost::log::core::get()->set_filter( boost::log::trivial::severity >= boost::log::trivial::info );
    else
        boost::log::core::get()->set_filter( boost::log::trivial::severity >= boost::log::trivial::warning );
    boost::log::add_common_attributes();
    boost::log::add_console_log(std::cout,
        boost::log::keywords::format = "[%TimeStamp%] %Message%",
        boost::log::keywords::auto_flush = true
    );

    // parse informative graph
    InformativeGraph g;
    while ( true )
    {
        size_t v, w, ag;
        if ( cin >> v >> w >> ag )
        {
            g.addEdge(v, w, ag);
        }
        else
            break;
    }

    // run algorithms
    runGreedy(g, false, showResult);
    runGreedy(g, true, showResult);
    return 0;
}
