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
    // TODO: read query k with test case file
    int max_number_of_tests = 100;
    int number_of_tests = 0;
    int query_k = -1;
    if (argc > 3) {
        stringstream(argv[3]) >> query_k;
    }

    cout << "1. Loading graph and test cases" << endl;
    PUNGraph net = TSnap::LoadEdgeList<PUNGraph>(argv[1], 0, 1);
    number_of_tests = 
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
    cout << "the maximum k of the graph is: " << max_net_k << endl;
    cout << "elapsed time: " << 
        double(clock() - start_time) / CLOCKS_PER_SEC << endl;
    start_time = clock();

    cout << "4. Build MST from generated graph" << endl;
    int num_cc = 
        construct_mst(net, edge_trussness, triangle_trussness, mst, max_net_k);
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
    construct_index_tree(mst, edge_trussness, triangle_trussness, 
            index_tree, index_hash);
    size_t inode_ignore_cnt = 0;
    for (eiid_map::const_iterator citer = index_hash.begin();
            citer != index_hash.end(); 
            ++ citer) {
        if (citer->second == -1)
            inode_ignore_cnt ++;
    }
    cout << "Index tree has " << index_tree.size() 
        << " nodes with " << inode_ignore_cnt 
        << " ignored nodes." << endl;
    cout << "Index hash table size is " << index_hash.size() << endl;
    cout << "elapsed time: " << 
        double(clock() - start_time) / CLOCKS_PER_SEC << endl;
    start_time = clock();

    cout << "6. K-Truss Query Processing" << endl;
    if (query_k > 0) {
        cout << query_k << "-Truss query" << endl;
        vector<exact_qr_set_type> truss_communities;
        vector<qr_set_type> truss_community_infos;

        for (size_t i = 0; i < testcases.size(); i ++) {
            exact_qr_set_type truss_community;
            truss_raw_query(truss_community, 
                    testcases[i], query_k,
                    net, edge_trussness);
            truss_communities.push_back(truss_community);
        }
        cout << "elapsed time for raw query(average): " << 
            double(clock() - start_time) / CLOCKS_PER_SEC / 
            number_of_tests << endl;
        start_time = clock();

        for (size_t i = 0; i < testcases.size(); i ++) {
            qr_set_type truss_community_info;
            vector<vid_type> query_vids; 
            query_vids.push_back(testcases[i]);
            truss_k_query(truss_community_info, query_vids, query_k,
                    net, index_tree, index_hash);
            truss_community_infos.push_back(truss_community_info);
        }
        cout << "elapsed time for info query(average): " << 
            double(clock() - start_time) / CLOCKS_PER_SEC / 
            number_of_tests << endl;
        start_time = clock();

        cout << "verifying results..." << endl;
        for (size_t i = 0; i < testcases.size(); i ++) {
            if (truss_community_infos[i].size() != truss_communities[i].size()) {
                cout << "ERROR: wrong number of communities for test case: "
                    << i << " with query vertex " << testcases[i] 
                    << " and query k = " << query_k << endl;
            }
            for (size_t j = 0; j < truss_communities[i].size(); j ++) {
                bool found = false;
                for (qr_set_type::iterator 
                        iter = truss_community_infos[i].begin();
                        iter != truss_community_infos[i].end();
                        ++ iter) {
                    if (truss_communities[i][j].find(edge_extractor(iter->iid)) != 
                            truss_communities[i][j].end()) {
                        found = true;
                        if (iter->size !=  truss_communities[i][j].size()) {
                            cout << "ERROR: wrong size of communities for test case: "
                                << i << " with query vertex " << testcases[i] 
                                << " and query k = " << query_k << endl;
                        }
                        break;
                    }
                }
                if (!found) {
                    cout << "ERROR: cannot find communities for test case: "
                        << i << " with query vertex " << testcases[i] 
                        << " and query k = " << query_k << endl;
                }

            } 
        }
    }
    /*
    else if (query_k == -1){
        cout << "Any-K-truss query" << endl;
    }
    else {
        cout << "Max-K-Truss query" << endl;
    }
    */
    else {
        cout << "Unsupported query type." << endl;
    }

    return 0;
}

