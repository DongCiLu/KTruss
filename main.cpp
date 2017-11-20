#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>

#include "common.hpp"
#include "decomposer.hpp"
#include "induced_graph.hpp"
#include "tree_index.hpp"
#include "query.hpp"
#include "testcase.hpp"
#include "archiver.hpp"
/*
#include "update.hpp"
#include "tcp_index.hpp"
*/

void print_n_update_timer(bool silent = false) {
    static clock_t last_time;
    if (!silent) {
        cout << "elapsed time: " << 
            double(clock() - last_time) / CLOCKS_PER_SEC << endl;
    }
    last_time = clock();
}

void do_queries(string graph_filename, 
        string checkpoint_dir, 
        string testcase_filename, 
        int query_k, 
        size_t n_queries) {
    vector<vid_type> testcases;
    cout << "1. Loading graph and testcases" << endl;
    PUNGraph net = TSnap::LoadEdgeList<PUNGraph>(
            graph_filename.c_str(), 0, 1);
    n_queries = load_testcases(testcase_filename, n_queries, testcases);
    cout << "Teset case size: " << n_queries << endl;
    print_n_update_timer();

    cout << "2. Loading indices" << endl;
    eint_map edge_trussness;
    counting_sorted_type sorted_edge_trussness; // not loaded
    PUNGraph mst;
    eint_map triangle_trussness;
    iidinode_map index_tree;
    eiid_map index_hash;
    load_edge_trussness(edge_trussness, sorted_edge_trussness,
        graph_filename, checkpoint_dir, false);
    load_mst(mst, triangle_trussness, encode_table, decode_table, 
            graph_filename, checkpoint_dir);
    load_index_tree(index_tree, index_hash, graph_filename, checkpoint_dir);
    cout << "edge trussness size: " << edge_trussness.size() << endl;
    cout << "triangle trussness size: " << triangle_trussness.size() << endl;
    cout << "encode table size: " << encode_table.size() << endl;
    cout << "decode table size: " << decode_table.size() << endl;
    cout << "index tree size: " << index_tree.size() << endl;
    cout << "index hash size: " << index_hash.size() << endl;
    print_n_update_timer();

    cout << "3. K-Truss Query Processing" << endl;
    if (query_k > 0) {
        cout << query_k << "-Truss query" << endl;
        vector<exact_qr_set_type> truss_communities;
        vector<qr_set_type> truss_community_infos;

        cout << "3.1 Starting raw query" << endl;
        for (size_t i = 0; i < testcases.size(); i ++) {
            exact_qr_set_type truss_community;
            truss_raw_query(truss_community, 
                    testcases[i], query_k,
                    net, edge_trussness);
            truss_communities.push_back(truss_community);
        }
        print_n_update_timer();

        cout << "3.2 Starting truss info query" << endl;
        for (size_t i = 0; i < testcases.size(); i ++) {
            qr_set_type truss_community_info;
            vector<vid_type> query_vids; 
            query_vids.push_back(testcases[i]);
            truss_k_query(truss_community_info, query_vids, query_k,
                    net, index_tree, index_hash);
            truss_community_infos.push_back(truss_community_info);
        }
        print_n_update_timer();

        cout << "verifying results..." << endl;
        size_t verification_error = 0;
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
                    if (std::find(truss_communities[i][j].begin(), 
                                truss_communities[i][j].end(),
                                vertex_extractor(edge_extractor(iter->iid))) !=
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
                    verification ++;
                    cout << "ERROR: cannot find communities for test case: "
                        << i << " with query vertex " << testcases[i] 
                        << " and query k = " << query_k << endl;
                }
            } 
        }
        if (verification_error == 0) 
            cout << "Success!" << endl;
    }
    else if (query_k == 0) {
        cout << "Any-K-truss query" << endl;
    }
    else if (query_k == -1) {
        cout << "Max-K-Truss query" << endl;
    }
    else {
        cout << "Unsupported query type." << endl;
    }
}

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

void generate_indices(string graph_filename, string checkpoint_dir) {
    unordered_set<eid_type> elist;
    eint_map edge_support;
    eint_map edge_trussness;

    PUNGraph mst = TUNGraph::New();
    eint_map triangle_trussness;

    iidinode_map index_tree;
    eiid_map index_hash;

    tcp_index_table_type tcp_index;

    bool silent = true;
    create_checkpoint_dir(checkpoint_dir);

    cout << "1. Loading graph ...." << endl;
    PUNGraph net = TSnap::LoadEdgeList<PUNGraph>(
            graph_filename.c_str(), 0, 1);
    cout << "Graph size: " << net->GetNodes() << 
        " " << net->GetEdges() << endl;
    for (TUNGraph::TEdgeI EI = net->BegEI(); EI < net->EndEI(); EI++) 
        elist.insert(edge_composer(EI.GetSrcNId(), EI.GetDstNId()));
    cout << "Edge list size: " << elist.size() << endl;
    print_n_update_timer();

    cout << "2. Compute support" << endl;
    slow_sorted_type sorted_edge_support; 
    compute_support(net, elist, edge_support, sorted_edge_support);
    string support_filename = graph_filename + "esupport";
    print_n_update_timer();
    cout << "edge support size: " << edge_support.size() << endl;
    cout << "sorted edge support size: " << sorted_edge_support.size() << endl;

    cout << "3. Truss Decomposition (Index Construction)" << endl;
    counting_sorted_type sorted_edge_trussness;
    int max_net_k = compute_trussness(
            net, edge_support, sorted_edge_support, 
            edge_trussness, sorted_edge_trussness);
    cout << "the maximum k of the graph is: " << max_net_k << endl;
    print_n_update_timer();
    save_edge_trussness(edge_trussness, sorted_edge_trussness, 
            graph_filename, checkpoint_dir);
    print_n_update_timer(silent);

    cout << "4. Build MST from generated graph" << endl;
    int num_cc = construct_mst(
            net, edge_trussness, sorted_edge_trussness, mst, triangle_trussness);
    check_mst(mst, net, num_cc);
    print_n_update_timer();
    save_mst(mst, triangle_trussness, 
            encode_table, decode_table,
            graph_filename, checkpoint_dir);
    print_n_update_timer(silent);

    cout << "5. Construct heirachical index" << endl;
    construct_index_tree(mst, edge_trussness, triangle_trussness, 
            index_tree, index_hash);
    check_index_tree(index_tree, index_hash);
    print_n_update_timer();
    save_index_tree(index_tree, index_hash, 
            graph_filename, checkpoint_dir);
    print_n_update_timer(silent);

    /*
    cout << "6. Construct tcp index" << endl;
    construct_tcp_index(net, edge_trussness, tcp_index);
    cout << "TCP index has " << tcp_index.size() << " nodes." << endl;
    print_n_update_timer();
    */

    cout << "edge trussness size: " << edge_trussness.size() << endl;
    cout << "triangle trussness size: " << triangle_trussness.size() << endl;
    cout << "encode table size: " << encode_table.size() << endl;
    cout << "decode table size: " << decode_table.size() << endl;
    cout << "index tree size: " << index_tree.size() << endl;
    cout << "index hash size: " << index_hash.size() << endl;
}

int main(int argc, char** argv){
    print_n_update_timer(true);

    if (argc < 3) {
        cout << "USAGE: ./main mode graph_fn [testcase_fn query_k n_queries]" << endl;
        return -1;
    }
    string mode = argv[1];
    string graph_filename = argv[2];
    string checkpoint_dir = "dataset/checkpoints";
    if (mode == "index") {
        generate_indices(graph_filename, checkpoint_dir);
    }
    else if (mode == "query") {
        if (argc < 6) {
            cout << "ERROR: missing testcase_fn or query_k or n_queries" << endl;
            return -1;
        }
        string testcase_filename = argv[3];
        int query_k = atoi(argv[4]); 
        size_t n_queries = atoi(argv[5]); 
        do_queries(graph_filename, checkpoint_dir, 
                testcase_filename, query_k, n_queries);
    }
    else {
        cout << "ERROR: wrong mode, only support index mode or query mode" << endl;
    }

    return 0;
}

