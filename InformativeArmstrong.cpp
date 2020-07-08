#include "InformativeGraph.h"

using namespace std;

void pickForced(InformativeGraph &g)
{
    for ( NodeID node : g.getForced() )
        g.pickNode(node);
}

void pickGreedy(InformativeGraph &g)
{
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
    // print result of parsing (basic debug)
    cout << g;
    return 0;
}
