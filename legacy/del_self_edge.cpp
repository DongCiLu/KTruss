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
    TFOut out(fn);
    net->Save(out);

    return 0;
}

