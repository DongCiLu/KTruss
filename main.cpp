#include <iostream>

#include "common.hpp"
#include "decomposer.hpp"
#include "induced_graph.hpp"
#include "tree_index.hpp"
#include "query.hpp"
#include "update.hpp"
#include "testcase.hpp"

int main(int argc, char** argv){
    eint_map edge_support;
    eint_map edge_trussness;

    PUNGraph mst = TUNGraph::New();
    eint_map triangle_trussness;

    iidinode_map index_tree;
    eiid_map index_hash;

    vector<vid_type> testcases;
    clock_t start_time = clock();

    if (argc < 3) {
        cout << 
            "USAGE: ./ktruss [graphfile] [testcasesfile]\n" << endl;
        return -1;
    }

    // the number of testcases is 
    // min(testcase file length, max_nmumber_of_tests)
    string testcase_file = argv[2];
    // TODO: set these two variables as input parameter with default value
    int max_number_of_tests = 100;
    int query_k = -1;
    if (argc > 3) {
        stringstream(argv[3]) >> query_k;
    }

    cout << "1. Loading graph and test cases" << endl;
    PUNGraph net = TSnap::LoadEdgeList<PUNGraph>(argv[1], 0, 1);
    load_testcases(testcase_file, testcases, max_number_of_tests);
    cout << "Graph size: " << net->GetNodes() << 
        " " << net->GetEdges() << endl;
    cout << "elapsed time: " << 
        double(clock() - start_time) / CLOCKS_PER_SEC << endl;
    start_time = clock();

    cout << "2. Compute support" << endl;
    set< pair<int, eid_type> > sorted_edge_support; // value-key
    compute_support(net, edge_support, sorted_edge_support);
    cout << "elapsed time: " << 
        double(clock() - start_time) / CLOCKS_PER_SEC << endl;
    start_time = clock();
          
    cout << "3. Truss Decomposition (Index Construction)" << endl;
    max_net_k = 
        compute_trussness(net, edge_support, sorted_edge_support, edge_trussness);
    cout << "elapsed time: " << 
        double(clock() - start_time) / CLOCKS_PER_SEC << endl;
    start_time = clock();

    cout << "4. Build MST from generated graph" << endl;
    int num_cc = 
        construct_mst(net, edge_trussness, triangle_trussness, mst);
    if (mst->GetNodes() != net->GetEdges() || 
            mst->GetEdges() != mst->GetNodes() - num_cc) { 
        cout << "Incorrect mst constructed." << endl;
        cout << mst->GetNodes() << "-" << net->GetEdges() << " " 
            << mst->GetEdges() << "-" << mst->GetNodes() - num_cc << endl;
    }
    else {
        cout << "MST has " << mst->GetNodes() << " nodes, "
            << mst->GetEdges() << " edges and "
            << num_cc << " connected components." << endl;
    }
    cout << "elapsed time: " << 
        double(clock() - start_time) / CLOCKS_PER_SEC << endl;
    start_time = clock();

    cout << "5. Construct heirachical index" << endl;
    construct_index_tree(mst, triangle_trussness, 
            index_tree, index_hash);
    cout << index_tree.size() << " " << index_hash.size() << endl;
    cout << "elapsed time: " << 
        double(clock() - start_time) / CLOCKS_PER_SEC << endl;
    start_time = clock();

    cout << "6. K-Truss Query Processing" << endl;
    if (query_k > 0) {
        cout << query_k << "-Truss query" << endl;
        int cnt = 0;
        vector< set<eid_type> > truss_communities;
        QR res;
        for (int i = 0; i < number_of_tests; i++) {
            if (cnt ++ % 10 == 9)
                cout << "." << flush;
                truss_communities = truss_raw_query(
                        query_k, testcases[i], net, edge_trussness);
                vector<vid_type> vids;
                vids.push_back(testcases[i]);
                //res = truss_k_query(
                        //query_k, vids, net, index_tree, index_hash);
        }
        cout << endl;
        cout << "elapsed time (average): " << 
            double(clock() - start_time) / CLOCKS_PER_SEC / 
            number_of_tests << endl;
        start_time = clock();
        cout << "noc: " << truss_communities.size() << endl;
        for (size_t i = 0; i < truss_communities.size(); i++)
            cout << truss_communities[i].size() << " ";
        cout << endl;
        cout << "res: " << res.size << endl;
    }
    else if (query_k == 0){
        cout << "Any-K-truss query" << endl;
    }
    else {
        cout << "Max-K-Truss query" << endl;
    }

    return 0;
}

