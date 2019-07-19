#include <iostream>
#include <string>
#include <set>
#include <unordered_set>
#include <utility>
#include <queue>
#include <boost/functional/hash.hpp>
#include "Snap.h"

using namespace std;

int main(int argc, char** argv) {
    PUNGraph net = TSnap::LoadEdgeList<PUNGraph>(argv[1], 0, 1);
    string fn(argv[1]);
    fn += "bfs";
    cout << fn << endl;
    TSnap::DelSelfEdges(net);

    unordered_set<int> unvisited;

    for (TUNGraph::TNodeI NI = net->BegNI(); NI < net->EndNI(); NI ++) {
        unvisited.insert(NI.GetId());
    }

    while(!unvisited.empty()) {
        int seed = *(unvisited.begin());
        unvisited.erase(seed);
        queue<int> fifo;
        fifo.push(seed);
        while(!fifo.empty()) {
            int cur = fifo.front();
            fifo.pop();
            for (int i = 0; i < net->GetNI(cur).GetDeg(); i ++) {
                int nbr = net->GetNI(cur).GetNbrNId(i);
                if (unvisited.find(nbr) != unvisited.end()) {
                    unvisited.erase(nbr);
                    fifo.push(nbr);
                    cout << cur << "  " << nbr << endl;
                }
            }
        }
    }

    return 0;
}

