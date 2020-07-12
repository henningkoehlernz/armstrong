#include <queue>
#include "InformativeGraph.h"
#include "OrderedTrie.h"

using namespace std;

void pickForced(InformativeGraph &g)
{
    for ( NodeID node : g.getForced() )
        g.pickNode(node);
}

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

// repeatedly pick node with largest (certain agree-set, degree) pair
void pickGreedy(InformativeGraph &g)
{
    std::priority_queue<GreedyNode> queue;
    const size_t nodeCount = g.nodeCount();
    // prepare queue
    for ( NodeID node = 0; node < nodeCount; node++ )
    {
        if ( g.degree(node) > 0 )
            queue.push(GreedyNode(g, node));
    }
    // repeated pick top element
    while ( !queue.empty() )
    {
        GreedyNode next = queue.top();
        queue.pop();
        // check that node data is still accurate
        GreedyNode currentValue(g, next.node);
        if ( next == currentValue )
        {
            if ( !g.picked(next.node) )
            {
                g.pickNode(next.node);
                // certainSize of neighbors may have increased
                for ( NodeID neighbor : g.getNeighbors(next.node) )
                    // any current neighbor will have degree > 0
                    queue.push(GreedyNode(g, neighbor));
            }
        }
        else if ( currentValue < next && currentValue.degree > 0 )
        {
            // re-enqueue for later processing
            queue.push(currentValue);
        }
    }
}

int main(int argc, char* argv[])
{
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
    pickForced(g);
    pickGreedy(g);
    // print result of parsing (basic debug)
    cout << g;
    cout << '(' << g.getPicked().size() << " nodes picked)" << endl;
    return 0;
}
