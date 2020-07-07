#include "InformativeGraph.h"

using namespace std;

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
    // print result of parsing (basic debug)
    cout << g;
    return 0;
}
