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
            double(clock() - last_time) / 
            (double(CLOCKS_PER_SEC) / 1000) << "ms." << endl;
    }
    last_time = clock();
}

void check_mst(const PUNGraph mst, 
        const PUNGraph net, 
        const int num_cc) {
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
    TSnap::DelSelfEdges(net);
    cout << "Graph size after remove self edges: " << net->GetNodes() << 
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
void verify_raw_info(vector<vid_type> &testcases,
        vector<exact_qr_set_type> &truss_communities,
        vector<qr_set_type> &truss_community_infos) {
    size_t verification_error = 0;
    for (size_t i = 0; i < testcases.size(); i ++) {
        if (truss_community_infos[i].size() != 
                truss_communities[i].size()) {
            cout << "ERROR: wrong number of communities for testcase: "
                << i << " with query vertex " << testcases[i] << endl;
        }
        for (qr_set_type::iterator 
                iter =  truss_community_infos[i].begin();
                iter != truss_community_infos[i].end();
                ++ iter) {
            bool found = false;
            for (size_t j = 0; j < truss_communities[i].size(); j ++) {
                if (std::find(truss_communities[i][j].begin(), 
                            truss_communities[i][j].end(),
                            vertex_extractor(
                                edge_extractor(iter->iid))) !=
                        truss_communities[i][j].end()) {
                    found = true;
                    if (iter->size != truss_communities[i][j].size()) {
                        cout << "ERROR: wrong size of communities" 
                            << " for test case: "<< i 
                            << " with query vertex " 
                            << testcases[i] << ", indexed size "
                            << iter->size << " actual size " 
                            << truss_communities[i][j].size() << endl;
                        cout << "Inode info: " << iter->iid << " " 
                            << iter->k << " " << iter->size << endl;
                    }
                    break;
                }
            }
            if (!found && !truss_community_infos[i].empty()) {
                verification_error ++;
                cout << "ERROR: cannot find communities for testcase: "
                    << i << " with query vertex " 
                    << testcases[i]  << endl;
            }
        } 
    }
    if (verification_error == 0) 
        cout << "Success!" << endl;
}

void verify_raw_exact(vector<vid_type> &testcases,
        vector<exact_qr_set_type> &truss_communities1,
        vector<exact_qr_set_type> &truss_communities2) {
    size_t verification_error = 0;
    for (size_t i = 0; i < testcases.size(); i ++) {
        if (truss_communities1[i].size() != 
                truss_communities2[i].size()) {
            cout << "ERROR: wrong number of communities for testcase: "
                << i << " with query vertex " << testcases[i] << endl;
            cout << "raw size: " << truss_communities1[i].size()
                << " exact size: " << truss_communities2[i].size() << endl;
        }
        community_type total_community1, total_community2;
        for (size_t j = 0; j < truss_communities1[i].size(); j ++) {
            total_community1.insert(total_community1.end(), 
                    truss_communities1[i][j].begin(),
                    truss_communities1[i][j].end());
        } 
        for (size_t j = 0; j < truss_communities2[i].size(); j ++) {
            total_community2.insert(total_community2.end(), 
                    truss_communities2[i][j].begin(),
                    truss_communities2[i][j].end());
        } 
        std::sort(total_community1.begin(), total_community1.end());
        std::sort(total_community2.begin(), total_community2.end());
        if (total_community1 != total_community2) {
            verification_error ++;
            cout << "failed to pass verification" << endl;
            cout << "raw size: " << total_community1.size() 
                << " exact size: " << total_community2.size() << endl;
            set< pair<vid_type, vid_type> > 
                unique_community1(total_community1.begin(), 
                        total_community1.end());
            set< pair<vid_type, vid_type> > 
                unique_community2(total_community2.begin(), 
                        total_community2.end());
            cout << "unique raw size: " << unique_community1.size()
                << " unique exact size: " << total_community2.size() << endl;
        }
    }
    if (verification_error == 0) 
        cout << "Success!" << endl;
}

/*
void special_test(eint_map &edge_trussness, PUNGraph net, 
        eint_map &triangle_trussness, PUNGraph mst) {
    eid_type e1 = edge_extractor(35306981);
    eid_type e2 = edge_extractor(42179835);
    pair<vid_type, vid_type> vpair1 = vertex_extractor(e1);
    pair<vid_type, vid_type> vpair2 = vertex_extractor(e2);
    cout << "edge 1: " << vpair1.first << " " << vpair1.second 
        << " with trussness: " << edge_trussness[e1] << endl;
    cout << "edge 2: " << vpair2.first << " " << vpair2.second 
        << " with trussness: " << edge_trussness[e2] << endl;

    check_mst(mst, net, 7581208);
    unordered_set<eid_type> visited_edges;
    map<int, size_t> truss_count;
    community_type truss_community;
    truss_raw_edge_query(truss_community, e1, 16, 
            net, edge_trussness, visited_edges, truss_count);
    cout << "16 size: " << truss_community.size() << endl;

    exact_qr_set_type truss_community_ext;
    qr_type res_node(35306981, 199, 16);
    qr_set_type res;
    res.push_back(res_node);
    truss_exact_query(truss_community_ext, 
            res, mst, triangle_trussness, truss_community);

    visited_edges.clear();
    truss_count.clear();
    truss_community.clear();
    truss_raw_edge_query(truss_community, e1, 10, 
            net, edge_trussness, visited_edges, truss_count);
    cout << "10 size: " << truss_community.size() << endl;
    if (std::find(truss_community.begin(), 
                truss_community.end(),
                vpair2) != truss_community.end()) {
        cout << "found vpair2 in truss 10" << endl;
    }
}
*/

void do_queries(string graph_filename, 
        string checkpoint_dir, 
        string testcase_filename, 
        int query_k, 
        size_t n_queries) {
    vector<vid_type> testcases;
    cout << "1. Loading graph and testcases" << endl;
    PUNGraph net = TSnap::LoadEdgeList<PUNGraph>(
            graph_filename.c_str(), 0, 1);
    TSnap::DelSelfEdges(net);
    n_queries = 
        load_testcases(testcase_filename, n_queries, testcases);
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

    // special_test(edge_trussness, net, triangle_trussness, mst);

    cout << "3. K-Truss Query Processing" << endl;
    if (query_k > 0) {
        cout << query_k << "-Truss query" << endl;
        vector<exact_qr_set_type> truss_communities1;
        vector<exact_qr_set_type> truss_communities2;
        vector<qr_set_type> truss_community_infos;

        size_t bucket_size = 100;
        cout << "3.1 Starting raw query" << endl;
        for (size_t i = 0; i < testcases.size(); i ++) {
            if (i % bucket_size == bucket_size - 1)
                print_n_update_timer();
            exact_qr_set_type truss_community;
            truss_raw_query(truss_community, 
                    testcases[i], query_k,
                    net, edge_trussness);
            truss_communities1.push_back(truss_community);
        }
        print_n_update_timer(true);

        cout << "3.2 Starting truss info query" << endl;
        for (size_t i = 0; i < testcases.size(); i ++) {
            if (i % bucket_size == bucket_size - 1)
                print_n_update_timer();
            qr_set_type truss_community_info;
            vector<vid_type> query_vids; 
            query_vids.push_back(testcases[i]);
            truss_k_query(truss_community_info, query_vids, query_k,
                    net, index_tree, index_hash);
            truss_community_infos.push_back(truss_community_info);
        }
        print_n_update_timer(true);

        cout << "3.2 Starting truss exact query" << endl;
        for (size_t i = 0; i < testcases.size(); i ++) {
            if (i % bucket_size == bucket_size - 1)
                print_n_update_timer();
            exact_qr_set_type truss_community;
            truss_exact_query(truss_community, 
                    truss_community_infos[i], 
                    mst, triangle_trussness);
            truss_communities2.push_back(truss_community);
        }
        print_n_update_timer(true);

        cout << "verifying results..." << endl;
        verify_raw_info(testcases, truss_communities1, 
                truss_community_infos);
        verify_raw_exact(testcases, truss_communities1, 
                truss_communities2);
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

void generate_testcases(string graph_filename, string testcase_dir) {
    cout << "1. Loading graph ...." << endl;
    PUNGraph net = TSnap::LoadEdgeList<PUNGraph>(
            graph_filename.c_str(), 0, 1);
    cout << "Graph size: " << net->GetNodes() << 
        " " << net->GetEdges() << endl;
    save_testcases(net, graph_filename, testcase_dir, "single_q");
    // save_testcases(net, graph_filename, testcase_dir, "multi_q");
}

int main(int argc, char** argv){
    print_n_update_timer(true);

    if (argc < 3) {
        cout << "USAGE: ./main mode graph_fn [testcase_fn query_k n_queries]" << endl;
        return -1;
    }
    string mode = argv[1];
    string graph_filename = argv[2];
    string checkpoint_dir = "datasets/checkpoints_test";
    string testcase_dir = "datasets/testcases_truss";

    if (mode == "index") {
        generate_indices(graph_filename, checkpoint_dir);
    }
    else if (mode == "testcase") {
        generate_testcases(graph_filename, testcase_dir);
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

