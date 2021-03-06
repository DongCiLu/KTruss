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
#include "tcp_index.hpp"
#include "equi_index.hpp"

//#define EXAMPLE
//#define VERIFY_RESULT
//#define TCP
#define EQUI
#define TEMP_CHANGE

void print_support_index_info(eint_map &edge_support, 
                              slow_sorted_type &sorted_edge_support) {
    cout << "edge support size: " 
         << edge_support.size() << endl;
    cout << "sorted edge support size: " 
         << sorted_edge_support.size() << endl;
#ifdef EXAMPLE
    for (auto iter = edge_support.begin();
            iter != edge_support.end();
            ++ iter) {
        auto vpair = vertex_extractor(iter->first);
        cout << vpair.first << " " << vpair.second 
            << ": " << iter->second << endl;
    }
#endif
}

void print_index_info(eint_map &edge_trussness,
                      unordered_map<eid_type, vid_type> &encode_table,
                      unordered_map<vid_type, eid_type> &decode_table,
                      iidinode_map &index_tree,
                      eiid_map &index_hash,
                      tcp_index_table_type &tcp_index,
                      equi_hash_type &equi_hash,
                      equi_index_type &equi_index,
                      int max_net_k = -1) {
    cout << "\nIndex information: " << endl;
    cout << "\tedge trussness size: " 
         << edge_trussness.size() << endl;
#ifdef EXAMPLE
    for (auto iter = edge_trussness.begin();
            iter != edge_trussness.end();
            ++ iter) {
        auto vpair = vertex_extractor(iter->first);
        cout << vpair.first << " " << vpair.second 
            << ": " << iter->second << endl;
    }
#endif
    cout << "\tencode table size: " 
         << encode_table.size() << endl;
    cout << "\tdecode table size: " 
         << decode_table.size() << endl;
    size_t it_edges = 0;
    for (auto iter = index_tree.begin();
            iter != index_tree.end();
            ++ iter) {
        if (iter->second.parent != -1) it_edges ++;
#ifdef EXAMPLE
        cout << iter->first << ": " 
            << iter->second.parent << " [" 
            << iter->second.k << "]" << endl;
#endif
    }
    cout << "\tindex tree has " 
         << index_tree.size() << " nodes, "
         << it_edges << " edges." << endl;
    cout << "\tindex hash size: " 
         << index_hash.size() << endl;
#ifdef EXAMPLE
    for (auto iter = index_hash.begin();
            iter != index_hash.end();
            ++ iter) {
        auto vpair1 = vertex_extractor(iter->first);
        //auto vpair2 = vertex_extractor(iter->second);
        cout << vpair1.first << " " << vpair1.second << ": " 
             << iter->second << endl;
             //<< vpair2.first << " " << vpair2.second << endl;
    }
#endif
    if (max_net_k != -1)
        cout << "\tthe maximum k of the graph is: " 
             << max_net_k << endl;
    cout << "\ttcp index has " 
         << tcp_index.size() << " nodes." << endl;
    size_t tcp_size = 0;
    for (tcp_index_table_type::iterator 
            iter = tcp_index.begin();
            iter != tcp_index.end();
            ++ iter) {
        tcp_size += iter->second.ego_triangle_trussness.size();
    }
    cout << "\ttcp index size: " << tcp_size << endl;
    size_t equi_hash_size = 0;
    for (auto entry: equi_hash) {
        equi_hash_size += entry.second.size();
    }
    cout << "\tequitruss hash size: " << equi_hash.size() 
        << " " << equi_hash_size << endl;
    cout << "\tequitruss super graph has "
        << equi_index.super_graph->GetNodes() << " nodes "
        << equi_index.super_graph->GetEdges() << " edges." << endl;
    size_t equi_size = 0;
    for (auto iter = equi_index.super_nodes.begin();
            iter != equi_index.super_nodes.end();
            ++ iter) {
        equi_size += iter->second.edge_list.size();
    }
    cout << "\tequitruss index size: " << equi_size << endl;
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

    // SNAP uses smart-pointers, no need to explicitly free graph objects.
    PUNGraph mst = TUNGraph::New();
    eint_map triangle_trussness;

    iidinode_map index_tree;
    eiid_map index_hash;

    tcp_index_table_type tcp_index;
    equi_hash_type equi_hash;
    equi_index_type equi_index;

    Timer t;
    create_checkpoint_dir(checkpoint_dir);

    cout << "\n1. Loading graph ...." << endl;
    cout << graph_filename.c_str() << endl;
    PUNGraph net = TSnap::LoadEdgeList<PUNGraph>(
            graph_filename.c_str(), 0, 1);
    cout << "Graph size: " << net->GetNodes() << 
        " " << net->GetEdges() << endl;
    // We use preprocessed graphs without any self edges
    // TSnap::DelSelfEdges(net);
    // cout << "Graph size after remove self edges: " 
    //      << net->GetNodes() << " "
    //      << net->GetEdges() << endl;
    for (TUNGraph::TEdgeI EI = net->BegEI(); EI < net->EndEI(); EI++) 
        elist.insert(edge_composer(EI.GetSrcNId(), EI.GetDstNId()));
    cout << "Edge list size: " << elist.size() << endl;
    t.print_n_update_timer();

    cout << "\n2. Compute support" << endl;
    slow_sorted_type sorted_edge_support; 
    compute_support(net, elist, edge_support, sorted_edge_support);
    string support_filename = graph_filename + "esupport";
    t.print_n_update_timer();
    print_support_index_info(edge_support, sorted_edge_support);

    cout << "\n3. Truss Decomposition (Index Construction)" << endl;
    counting_sorted_type sorted_edge_trussness;
    int max_net_k = compute_trussness(
            net, edge_support, sorted_edge_support, 
            edge_trussness, sorted_edge_trussness);
    t.print_n_update_timer();
    save_edge_trussness(edge_trussness, sorted_edge_trussness, 
            graph_filename, checkpoint_dir);
    t.update_timer();

    cout << "\n4. Build MST from generated graph" << endl;
    int num_cc = construct_mst(
            net, edge_trussness, sorted_edge_trussness, 
            mst, triangle_trussness);
    check_mst(mst, net, num_cc);
    t.print_n_update_timer();
    save_coding_table(encode_table, decode_table,
            graph_filename, checkpoint_dir);
    t.update_timer();

    cout << "\n5. Construct heirachical index" << endl;
    construct_index_tree(mst, edge_trussness, triangle_trussness, 
            index_tree, index_hash);
    check_index_tree(index_tree, index_hash);
    t.print_n_update_timer();
    save_index_tree(index_tree, index_hash, 
            graph_filename, checkpoint_dir);
    t.update_timer();

    cout << "\n6. Construct tcp index" << endl;
    construct_tcp_index(net, edge_trussness, tcp_index);
    t.print_n_update_timer();
    save_tcp_index(tcp_index, graph_filename, checkpoint_dir);
    t.update_timer();

    cout << "\n7. Construct equitruss index" << endl;
    // NOTE: net is destroyed after equi index construction
    construct_equi_index(net, edge_trussness, equi_hash, equi_index);
    t.print_n_update_timer();
    save_equi_index(equi_hash, equi_index, graph_filename, checkpoint_dir);
    t.update_timer();

    print_index_info(edge_trussness, 
                     encode_table, decode_table,
                     index_tree, index_hash,
                     tcp_index, 
                     equi_hash, equi_index, max_net_k); 
}

void compressor(string graph_filename, string checkpoint_dir) {
    cout << "\n1. Loading indices" << endl;
    iidinode_map index_tree;
    eiid_map index_hash;
    load_coding_table(encode_table, decode_table, 
            graph_filename, checkpoint_dir);
    load_index_tree(index_tree, index_hash, 
            graph_filename, checkpoint_dir);

    cout << "\n2. Compressing indices" << endl;
    compress(index_tree, index_hash, graph_filename, checkpoint_dir);
}

void verify_raw_info(vector<vector<vid_type>> &testcases,
        vector<exact_qr_set_type> &truss_communities,
        vector<qr_set_type> &truss_community_infos) {
    size_t verification_error = 0;
    for (size_t i = 0; i < testcases.size(); i ++) {
        if (truss_community_infos[i].size() != 
                truss_communities[i].size()) {
            cout << "ERROR: wrong number of communities for testcase: "
                << i << " with query vertex ";
            for (vid_type v: testcases[i])
                cout << v << " ";
            cout << endl;
            cout << "***2 " << truss_community_infos[i].size() 
                << " " << truss_communities[i].size() << endl;
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
                            << " with query vertex "; 
                        for (vid_type v: testcases[i])
                            cout << v << " ";
                        cout << ", indexed size "
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
                    << i << " with query vertex "; 
                for (vid_type v: testcases[i])
                    cout << v << " ";
                cout << endl;
            }
        } 
    }
    if (verification_error == 0) 
        cout << "Success!" << endl;
}

void verify_raw_exact(vector<vector<vid_type>> &testcases,
        vector<exact_qr_set_type> &truss_communities1,
        vector<exact_qr_set_type> &truss_communities2) {
    size_t verification_error = 0;
    for (size_t i = 0; i < testcases.size(); i ++) {
        if (truss_communities1[i].size() != 
                truss_communities2[i].size()) {
            cout << "ERROR: wrong number of communities for testcase: "
                << i << " with query vertex "; 
            for (vid_type v: testcases[i])
                cout << v << " ";
            cout << endl;
            cout << "raw number of communities: " 
                << truss_communities1[i].size()
                << " exact number of communities: " 
                << truss_communities2[i].size() << endl;
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
            cout << "raw community total size: " << total_community1.size() 
                << " exact community total size: " << total_community2.size() << endl;
            set< pair<vid_type, vid_type> > 
                unique_community1(total_community1.begin(), 
                        total_community1.end());
            set< pair<vid_type, vid_type> > 
                unique_community2(total_community2.begin(), 
                        total_community2.end());
            cout << "unique raw community size: " << unique_community1.size()
                << " unique exact community size: " << total_community2.size() << endl;
        }
    }
    if (verification_error == 0) 
        cout << "Success!" << endl;
}

void do_queries(string graph_filename, 
        string checkpoint_dir, 
        string testcase_filename, 
        int query_k, 
        size_t n_requested_queries) {
    Timer t;
    vector<vector<vid_type>> testcases;
    /* 
     * the exact query cache is only for runtime analysis purpose,
     * when the exact query result is required for verification, 
     * this cache should be disabled.
     */
#ifdef VERIFY_RESULT
    bool cache_flag = false;
#else
    bool cache_flag = true;
#endif
    unordered_map<inode_id_type, double> exact_query_cache;
    
    cout << "\n1. Loading graph and testcases" << endl;
    PUNGraph net = TSnap::LoadEdgeList<PUNGraph>(
            graph_filename.c_str(), 0, 1);
    size_t n_queries = load_testcases(
            testcase_filename, n_requested_queries, testcases);
    cout << "Teset case size: " << n_queries << endl;
    t.print_n_update_timer();

    cout << "\n2. Loading indices" << endl;
    eint_map edge_trussness;
    counting_sorted_type sorted_edge_trussness; // not loaded
    iidinode_map index_tree;
    eiid_map index_hash;
    tcp_index_table_type tcp_index;
    equi_hash_type equi_hash;
    equi_index_type equi_index;
    load_edge_trussness(edge_trussness, sorted_edge_trussness,
        graph_filename, checkpoint_dir, false);
    // load mst for path finding purpose at the moment
    load_coding_table(encode_table, decode_table, 
            graph_filename, checkpoint_dir);
    load_index_tree(index_tree, index_hash, 
            graph_filename, checkpoint_dir);
#ifdef TCP
    load_tcp_index(tcp_index, graph_filename, checkpoint_dir);
#endif
#ifdef EQUI
    load_equi_index(equi_hash, equi_index, graph_filename, checkpoint_dir);
#endif
    print_index_info(edge_trussness, 
                     encode_table, decode_table,
                     index_tree, index_hash,
                     tcp_index, 
                     equi_hash, equi_index); 
    t.print_n_update_timer();

    cout << "\n3. K-Truss Query Processing" << endl;
    size_t bucket_size = 100;
    if (query_k > 0) {
        cout << query_k << "-Truss query" << endl;
#ifdef VERIFY_RESULT
        vector<exact_qr_set_type> truss_communities1;
        vector<exact_qr_set_type> truss_communities2;
        vector<exact_qr_set_type> truss_communities3;
#endif
        vector<qr_set_type> truss_community_infos;

        /*
        cout << "3.1 Starting raw query" << endl;
        for (size_t i = 0; i < testcases.size(); i ++) {
            if (i % bucket_size == bucket_size - 1)
                t.print_n_update_timer();
            exact_qr_set_type truss_community;
            truss_raw_query(truss_community, 
                    testcases[i][0], query_k,
                    net, edge_trussness);
            truss_communities1.push_back(truss_community);
        }
        t.update_timer();
        */

        cout << "3.2 Starting truss info query" << endl;
        for (size_t i = 0; i < testcases.size(); i ++) {
            if (i % bucket_size == bucket_size - 1)
                t.print_n_update_timer();
            qr_set_type truss_community_info;
            truss_k_query(truss_community_info, 
                    testcases[i], query_k,
                    net, index_tree, index_hash);
            truss_community_infos.push_back(truss_community_info);
        }
        t.update_timer();

#ifndef TEMP_CHANGE
        cout << "3.3 Starting truss exact query" << endl;
        double total_time = 0;
        for (size_t i = 0; i < testcases.size(); i ++) {
            if (i % bucket_size == bucket_size - 1) {
                t.print_n_update_timer();
                cout << "\telapsed time without cache " 
                     << total_time << endl;
                total_time = 0;
            }
            exact_qr_set_type truss_community;
            /*
            total_time += truss_exact_query_alternative(truss_community, 
                    truss_community_infos[i], 
                    mst, triangle_trussness,
                    exact_query_cache, cache_flag);
            */
            total_time += truss_exact_query(truss_community, 
                    truss_community_infos[i], index_tree,
                    exact_query_cache, cache_flag);
#ifdef VERIFY_RESULT
            truss_communities2.push_back(truss_community);
#endif
        }
        t.update_timer();
#endif

#ifdef TEMP_CHANGE
        if (!testcases.empty()) {
#else
        if (!testcases.empty() && testcases[0].size() == 1) {
#endif
#ifdef TCP
            // single query vertex case
            cout << "3.4 Starting tcp query" << endl;
            for (size_t i = 0; i < testcases.size(); i ++) {
                if (i % bucket_size == bucket_size - 1)
                    t.print_n_update_timer();
                exact_qr_set_type truss_community;
                truss_tcp_query(truss_community, 
                        testcases[i][0], query_k,
                        net, edge_trussness, tcp_index);
#ifdef VERIFY_RESULT
                truss_communities3.push_back(truss_community);
#endif
            }
            t.update_timer();
#endif

#ifdef EQUI
            cout << "3.4 Starting equi query" << endl;
            for (size_t i = 0; i < testcases.size(); i ++) {
                if (i % bucket_size == bucket_size - 1)
                    t.print_n_update_timer();
                exact_qr_set_type truss_community;
                /*
                // single query vertex case
                truss_equi_query(truss_community, 
                        testcases[i][0], query_k,
                        edge_trussness, equi_hash, equi_index);
                */
                // multiple query vertex case
                unordered_set<vid_type> truss_info_communities;
                truss_equi_multi_info_query(truss_info_communities, 
                        testcases[i], query_k,
                        edge_trussness, equi_hash, equi_index);
#ifdef VERIFY_RESULT
                truss_communities3.push_back(truss_community);
#endif
            }
            t.update_timer();
#endif

#ifdef VERIFY_RESULT
            cout << "3.5 Verifying results..." << endl;
            verify_raw_info(testcases, truss_communities3, 
                    truss_community_infos);
            verify_raw_exact(testcases, truss_communities3, 
                    truss_communities2);
#endif
        }
    }
    else if (query_k == 0) {
        cout << "Any-K-truss query" << endl;
        vector<qr_set_type> truss_community_infos;

        cout << "3.2 Starting truss info query" << endl;
        for (size_t i = 0; i < testcases.size(); i ++) {
            if (i % bucket_size == bucket_size - 1)
                t.print_n_update_timer();
            qr_set_type truss_community_info;
            truss_anyk_query(truss_community_info, testcases[i], 
                    net, index_tree, index_hash);
            truss_community_infos.push_back(truss_community_info);
        }
        t.update_timer();

        cout << "3.3 Starting truss exact query" << endl;
        double total_time = 0;
        for (size_t i = 0; i < testcases.size(); i ++) {
            if (i % bucket_size == bucket_size - 1) {
                t.print_n_update_timer();
                cout << "\telapsed time without cache " 
                     << total_time << endl;
                total_time = 0;
            }
            exact_qr_set_type truss_community;
            /*
            total_time += truss_exact_query_alternative(truss_community, 
                    truss_community_infos[i], 
                    mst, triangle_trussness,
                    exact_query_cache, cache_flag);
            */
            total_time += truss_exact_query(truss_community, 
                    truss_community_infos[i], index_tree,
                    exact_query_cache, cache_flag);
        }
        t.update_timer();
    }
    else if (query_k == -1) {
        cout << "Max-K-Truss query" << endl;
        vector<qr_set_type> truss_community_infos;

        cout << "3.2 Starting truss info query" << endl;
        for (size_t i = 0; i < testcases.size(); i ++) {
            if (i % bucket_size == bucket_size - 1)
                t.print_n_update_timer();
            qr_set_type truss_community_info;
            truss_maxk_query(truss_community_info, testcases[i], 
                    net, index_tree, index_hash);
            truss_community_infos.push_back(truss_community_info);
        }
        t.update_timer();

        cout << "3.3 Starting truss exact query" << endl;
        double total_time = 0;
        for (size_t i = 0; i < testcases.size(); i ++) {
            if (i % bucket_size == bucket_size - 1) {
                t.print_n_update_timer();
                cout << "\telapsed time without cache " 
                     << total_time << endl;
                total_time = 0;
            }
            exact_qr_set_type truss_community;
            /*
            total_time += truss_exact_query_alternative(truss_community, 
                    truss_community_infos[i], 
                    mst, triangle_trussness,
                    exact_query_cache, cache_flag);
            */
            total_time += truss_exact_query(truss_community, 
                    truss_community_infos[i], index_tree,
                    exact_query_cache, cache_flag);
        }
        t.update_timer();
    }
    else if (query_k == -2 && !testcases.empty() && 
            testcases[0].size() == 2) {
        cout << "Truss path query" << endl;
        vector<qr_set_type> truss_community_infos;

        cout << "3.2 Starting truss info query" << endl;
        for (size_t i = 0; i < testcases.size(); i ++) {
            if (i % bucket_size == bucket_size - 1)
                t.print_n_update_timer();
            qr_set_type truss_community_info;
            truss_maxk_query(truss_community_info, testcases[i], 
                    net, index_tree, index_hash);
            truss_community_infos.push_back(truss_community_info);
        }
        t.update_timer();

        cout << "3.3 Starting truss path query" << endl;
        for (size_t i = 0; i < testcases.size(); i ++) {
            if (i % bucket_size == bucket_size - 1) 
                t.print_n_update_timer();
            // record vid of mst graph, 
            // can be converted to eid in original graph later
            vector<vid_type> maximin_truss_path;
            /*
            truss_path_query(maximin_truss_path, 
                    truss_community_infos[i], 
                    mst, triangle_trussness,
                    testcases[i][0], testcases[i][1]);
            */
            truss_path_query(maximin_truss_path, 
                    truss_community_infos[i], 
                    index_tree, index_hash,
                    testcases[i][0], testcases[i][1]);
        }
        t.update_timer();
    }
    else if (query_k == -3 && !testcases.empty() &&
            testcases[0].size() == 1) {
        cout << "Truss boundary query" << endl;
        vector<qr_set_type> truss_community_infos;

        cout << "3.1 Starting truss info query" << endl;
        for (size_t i = 0; i < testcases.size(); i ++) {
            if (i % bucket_size == bucket_size - 1)
                t.print_n_update_timer();
            qr_set_type truss_community_info;
            truss_maxk_query(truss_community_info, testcases[i], 
                    net, index_tree, index_hash);
            truss_community_infos.push_back(truss_community_info);
        }
        t.update_timer();

        /*
        // this definition is not suitable for community boundary.
        cout << "3.2 Prepare boundary edge table" << endl;
        // find edges that is neighbor of edges that does not belong to any community.
        unordered_set<eid_type> boundary_table;
        for (TUNGraph::TEdgeI EI = net->BegEI(); EI < net->EndEI(); EI++) {
            vid_type low, high;
            get_low_high_deg_vertices(
                    net, EI.GetSrcNId(), EI.GetDstNId(), low, high);
            for (int i = 0; i < net->GetNI(low).GetDeg(); i++) {
                vid_type w = net->GetNI(low).GetNbrNId(i);
                if (net->IsEdge(w, high)) {
                    eid_type e1 = edge_composer(low, w);
                    eid_type e2 = edge_composer(high, w);
                }
            }
        }
        t.update_timer();
        */

        cout << "3.3 Starting truss boundary query" << endl;
        double total_time = 0;
        double total_k = 0;
        double total_len = 0;
        int total_cnt = 0;
        for (size_t i = 0; i < testcases.size(); i ++) {
            if (i % bucket_size == bucket_size - 1) {
                t.print_n_update_timer();
                cout << "\telapsed time without cache " 
                     << total_time << endl;
                total_time = 0;
            }
            // record vid of mst graph, 
            // can be converted to eid in original graph later
            vector<vid_type> truss_boundary;
            total_time += truss_boundary_query(truss_boundary, 
                    truss_community_infos[i], 
                    index_tree, index_hash, 
                    exact_query_cache, cache_flag);
            int boundary_length = cache_flag ? 
                exact_query_cache[truss_community_infos[i][0].iid] : truss_boundary.size();
            total_cnt ++;
            total_len += boundary_length;
            total_k += truss_community_infos[i][0].k;
            // the cache will be used to store boundary length instead of time
            cout << "TESTCASE " << i 
                 << " boundary length " << boundary_length
                 << " trussness " << truss_community_infos[i][0].k << endl;
        }
        cout << "Final results for boundary search: "  
             << total_cnt << " valid queries with " 
             << total_len << " total boundary length and " 
             << total_k << " total trussness." << endl;

        t.update_timer();
    }
    else {
        cout << "Unsupported query type." << endl;
    }
}

void generate_testcases(string graph_filename, 
                        string testcase_dir,
                        size_t num_query_vertices) {
    cout << "1. Loading graph ...." << endl;
    PUNGraph net = TSnap::LoadEdgeList<PUNGraph>(
            graph_filename.c_str(), 0, 1);
    cout << "Graph size: " << net->GetNodes() << 
        " " << net->GetEdges() << endl;
    save_testcases(net, graph_filename, 
            testcase_dir, num_query_vertices);
}

void print_usage() {
    cout << "USAGE: ./main mode graph_filepath "
         << "[num_query_vertices query_k n_queries]" << endl;
    cout << "Possible mode: index, testcase, query" << endl;
    return;
}

int main(int argc, char** argv){
    if (argc < 3) {
        print_usage();
        return -1;
    }
    string mode = argv[1];
    string graph_filename = argv[2];
    string checkpoint_dir = "datasets/checkpoints";
    string testcase_dir = "datasets/testcases_truss";

    if (mode == "index") {
        generate_indices(graph_filename, checkpoint_dir);
    }
    else if (mode == "testcase") {
        if (argc < 4) {
            print_usage();
            return -1;
        }
        size_t num_query_vertices = atoi(argv[3]);
        generate_testcases(graph_filename, testcase_dir, 
                num_query_vertices);
    }
    else if (mode == "query") {
        if (argc < 6) {
            print_usage();
            return -1;
        }
        size_t num_query_vertices = atoi(argv[3]);
        string testcase_filename = get_testcase_filename(
                graph_filename, testcase_dir, num_query_vertices);
        int query_k = atoi(argv[4]); 
        size_t n_queries = atoi(argv[5]); 
        do_queries(graph_filename, checkpoint_dir, 
                testcase_filename, query_k, n_queries);
    }
    else if (mode == "compress") {
        /* calculate minimum space required to store the index */
        compressor(graph_filename, checkpoint_dir);
    }
    else if (mode == "equi") {
        generate_equi_index_from_existing_index(graph_filename, checkpoint_dir);
    }
    else {
        print_usage();
    }

    cout << "\nDone.\n" << endl;
    return 0;
}

