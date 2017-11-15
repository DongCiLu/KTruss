#include <iostream>
#include <fstream>

#include "common.hpp"
#include "decomposer.hpp"
#include "induced_graph.hpp"
#include "tree_index.hpp"
#include "query.hpp"
#include "update.hpp"
#include "testcase.hpp"
#include "tcp_index.hpp"

void print_n_update_timer(bool init = false) {
    static clock_t last_time;
    if (!init) {
        cout << "elapsed time: " << 
            double(clock() - last_time) / CLOCKS_PER_SEC << endl;
    }
    last_time = clock();
}

/*
void do_queries(const string &testcase_filename) {
    int max_number_of_tests = 100;
    int number_of_tests = 0;
    int query_k = -1;
    // the number of testcases is 
    // min(testcase file length, max_nmumber_of_tests)
    string testcase_file = argv[2];

    if (argc > 3) {
        stringstream(argv[3]) >> query_k;
    }


    vector<vid_type> testcases;

    number_of_tests = 
        load_testcases(testcase_file, testcases, max_number_of_tests);
    cout << "Teset case size: " << number_of_tests << endl;

    cout << "7. K-Truss Query Processing" << endl;
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
    else if (query_k == -1){
        cout << "Any-K-truss query" << endl;
    }
    else {
        cout << "Max-K-Truss query" << endl;
    }
    else {
        cout << "Unsupported query type." << endl;
    }
}
*/

void check_mst(const PUNGraph mst, const PUNGraph net, const int num_cc) {
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
}

void check_index_tree(const iidinode_map &index_tree, 
        const eiid_map &index_hash) {
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
}

/*
void save_indices(string &filename, eint_map &edge_support) {
    hashmap<int, int> a;
    std::ofstream ofs(filename.c_str());
    boost::archive::text_oarchive oa(ofs);
    oa << a;
}
*/

void generate_indices(const string &graph_filename) {
    eint_map edge_support;
    eint_map edge_trussness;

    PUNGraph mst = TUNGraph::New();
    eint_map triangle_trussness;

    iidinode_map index_tree;
    eiid_map index_hash;

    tcp_index_table_type tcp_index;

    cout << "1. Loading graph ...." << endl;
    PUNGraph net = TSnap::LoadEdgeList<PUNGraph>(
            graph_filename.c_str(), 0, 1);
    cout << "Graph size: " << net->GetNodes() << 
        " " << net->GetEdges() << endl;
    print_n_update_timer();

    cout << "2. Compute support" << endl;
    set< pair<int, eid_type> > sorted_edge_support; // value-key
    compute_support(net, edge_support, sorted_edge_support);
    string support_filename = graph_filename + "esupport";
    //save_indices(support_filename, edge_support);
    print_n_update_timer();

    cout << "edge support size: " << edge_support.size() << " * " << 3 * 4 << endl;
    cout << "sorted edge support size: " << sorted_edge_support.size() << " * " << 3 * 4 << endl;

    cout << "3. Truss Decomposition (Index Construction)" << endl;
    max_net_k = compute_trussness(
            net, edge_support, sorted_edge_support, edge_trussness);
    cout << "the maximum k of the graph is: " << max_net_k << endl;
    print_n_update_timer();

    cout << "4. Build MST from generated graph" << endl;
    int num_cc = construct_mst(
            net, edge_trussness, triangle_trussness, mst, max_net_k);
    check_mst(mst, net, num_cc);
    print_n_update_timer();

    cout << "5. Construct heirachical index" << endl;
    construct_index_tree(mst, edge_trussness, triangle_trussness, 
            index_tree, index_hash);
    check_index_tree(index_tree, index_hash);
    print_n_update_timer();

    /*
    cout << "6. Construct tcp index" << endl;
    construct_tcp_index(net, edge_trussness, tcp_index);
    cout << "TCP index has " << tcp_index.size() << " nodes." << endl;
    print_n_update_timer();
    */

    cout << "edge trussness size: " << edge_trussness.size() << " * " <<  3 * 4 << endl;
    cout << "triangle trussness size: " << triangle_trussness.size() << " * " << 3 * 4 << endl;
    cout << "encode table size: " << encode_table.size() << " * " <<  3 * 4 << endl;
    cout << "decode table size: " << decode_table.size() << " * " <<  3 * 4 << endl;
    cout << "index tree size: " << index_tree.size() << " * " <<  4 * 4 << endl;
    cout << "index hash size: " << index_hash.size() << " * " <<  3 * 4 << endl;
}

int main(int argc, char** argv){
    bool init = true;
    print_n_update_timer(init);

    generate_indices(argv[1]);

    return 0;
}

