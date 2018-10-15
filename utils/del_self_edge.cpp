#include <iostream>
#include <string>
#include "Snap.h"

using namespace std;

int main(int argc, char** argv) {
    PUNGraph net = TSnap::LoadEdgeList<PUNGraph>(argv[1], 0, 1);
    string fn(argv[1]);
    fn += "nse";
    cout << fn << endl;
    TSnap::DelSelfEdges(net);
    TSnap::SaveEdgeList(net, fn.c_str());

    return 0;
}

