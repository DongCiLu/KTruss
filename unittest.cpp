#include <algorithm>
#include "gtest/gtest.h"

#include "common.hpp"
#include "testcase.hpp"
#include "decomposer.hpp"
#include "induced_graph.hpp"
#include "tree_index.hpp"
#include "query.hpp"
#include "archiver.hpp"
/*
#include "tcp_index.hpp"
*/


// TODO: design a new example
 
string graph_fn = "datasets/testgraph_truss.txt"; // contains 10 vertices and 21 edges
string testcase_fn = "datasets/testcases_truss/unittest.txt";
vector<vid_type> testcases;
size_t num_testcases = 0;

PUNGraph net;
unordered_set<eid_type> elist;

eint_map edge_support;
slow_sorted_type sorted_edge_support; 
eint_map edge_trussness;
counting_sorted_type sorted_edge_trussness;

PUNGraph mst;
eint_map triangle_trussness;

iidinode_map index_tree;
eiid_map index_hash;

tcp_index_table_type tcp_index;

TEST(TestcasesLoadingTest, SingleQueryVertex) {
    // when required number of testcases is available in the provided file
    testcases.clear();
    num_testcases = 3;
    EXPECT_EQ(3, load_testcases(testcase_fn, testcases, num_testcases));
    // when required number of testcases is not available in the provided file
    testcases.clear();
    num_testcases = 9;
    EXPECT_EQ(8, load_testcases(testcase_fn, testcases, num_testcases));
}

TEST(TestcasesLoadingTest, MultipleQueryVertices) {
    // test multiple query vertex case
    testcases.clear();
    num_testcases = 5;
    ASSERT_EQ(-1, load_testcases(testcase_fn, testcases, num_testcases, "random_q"));
    testcases.clear();
    num_testcases = 5;
    ASSERT_EQ(-1, load_testcases(testcase_fn, testcases, num_testcases, "multi_q"));
}

TEST(DecomposerTest, EdgeSupportTest) {
    compute_support(net, elist, edge_support, sorted_edge_support);
    eint_map exp_edge_support;

    exp_edge_support.insert(make_pair(edge_composer(0, 1), 1));
    exp_edge_support.insert(make_pair(edge_composer(0, 2), 1));
    exp_edge_support.insert(make_pair(edge_composer(1, 2), 2));
    exp_edge_support.insert(make_pair(edge_composer(1, 4), 4));
    exp_edge_support.insert(make_pair(edge_composer(1, 3), 3));
    exp_edge_support.insert(make_pair(edge_composer(3, 4), 3));
    exp_edge_support.insert(make_pair(edge_composer(1, 8), 3));
    exp_edge_support.insert(make_pair(edge_composer(3, 8), 3));
    exp_edge_support.insert(make_pair(edge_composer(4, 8), 3));
    exp_edge_support.insert(make_pair(edge_composer(1, 7), 3));
    exp_edge_support.insert(make_pair(edge_composer(3, 7), 3));
    exp_edge_support.insert(make_pair(edge_composer(7, 8), 3));
    exp_edge_support.insert(make_pair(edge_composer(4, 7), 3));
    exp_edge_support.insert(make_pair(edge_composer(2, 4), 2));
    exp_edge_support.insert(make_pair(edge_composer(2, 5), 1));
    exp_edge_support.insert(make_pair(edge_composer(4, 5), 3));
    exp_edge_support.insert(make_pair(edge_composer(4, 6), 2));
    exp_edge_support.insert(make_pair(edge_composer(5, 6), 2));
    exp_edge_support.insert(make_pair(edge_composer(9, 6), 2));
    exp_edge_support.insert(make_pair(edge_composer(4, 9), 2));
    exp_edge_support.insert(make_pair(edge_composer(5, 9), 2));

    ASSERT_EQ(exp_edge_support, edge_support);
}

TEST(DecomposerTest, EdgeTrussnessTest) {
    max_net_k = compute_trussness(
            net, edge_support, sorted_edge_support, 
            edge_trussness, sorted_edge_trussness);
    eint_map exp_edge_trussness;

    exp_edge_trussness[edge_composer(0, 1)] = 3;
    exp_edge_trussness[edge_composer(0, 2)] = 3;
    exp_edge_trussness[edge_composer(2, 5)] = 3;
    // exp_edge_trussness[edge_composer(1, 2)] = 4;
    // exp_edge_trussness[edge_composer(4, 2)] = 4;
    exp_edge_trussness[edge_composer(1, 2)] = 3;
    exp_edge_trussness[edge_composer(4, 2)] = 3;
    exp_edge_trussness[edge_composer(4, 5)] = 4;
    exp_edge_trussness[edge_composer(4, 6)] = 4;
    exp_edge_trussness[edge_composer(5, 6)] = 4;
    exp_edge_trussness[edge_composer(4, 9)] = 4;
    exp_edge_trussness[edge_composer(5, 9)] = 4;
    exp_edge_trussness[edge_composer(6, 9)] = 4;
    exp_edge_trussness[edge_composer(4, 1)] = 5;
    exp_edge_trussness[edge_composer(3, 1)] = 5;
    exp_edge_trussness[edge_composer(4, 8)] = 5;
    exp_edge_trussness[edge_composer(3, 8)] = 5;
    exp_edge_trussness[edge_composer(1, 8)] = 5;
    exp_edge_trussness[edge_composer(4, 3)] = 5;
    exp_edge_trussness[edge_composer(1, 7)] = 5;
    exp_edge_trussness[edge_composer(3, 7)] = 5;
    exp_edge_trussness[edge_composer(7, 8)] = 5;
    exp_edge_trussness[edge_composer(4, 7)] = 5;

    ASSERT_EQ(exp_edge_trussness, edge_trussness);
    ASSERT_EQ(6, sorted_edge_trussness.size());
    size_t sorted_cnt = 0;
    for (size_t i = 0; i < sorted_edge_trussness.size(); i ++) {
        sorted_cnt += sorted_edge_trussness[i].size();
    }
    ASSERT_EQ(21, sorted_cnt);
}

TEST(MSTConstructionTest, ConstructionTest) {
    mst = TUNGraph::New();
    int num_cc = construct_mst(
            net, edge_trussness, sorted_edge_trussness, mst, triangle_trussness);
    ASSERT_EQ(mst->GetNodes(), net->GetEdges());
    ASSERT_EQ(mst->GetEdges(), mst->GetNodes() - num_cc);
    ASSERT_EQ(1, num_cc);
    ASSERT_EQ(21, mst->GetNodes());
    ASSERT_EQ(20, triangle_trussness.size());

    for (TUNGraph::TEdgeI EI = mst->BegEI(); EI < mst->EndEI(); EI++) {
        vid_type u1 = EI.GetSrcNId();
        vid_type u2 = EI.GetDstNId();
        eid_type e1 = edge_extractor(u1);
        eid_type e2 = edge_extractor(u2);
        eid_type mst_e = edge_composer(u1, u2);
            
        int hyper_e_trussness = triangle_trussness[mst_e];
        int hyper_v1_trussness = edge_trussness[e1];
        int hyper_v2_trussness = edge_trussness[e2];

        ASSERT_EQ(hyper_e_trussness, std::min(hyper_v1_trussness, hyper_v2_trussness));
    }
}

TEST(TreeIndexConstructionTest, ConstructionTest) {
    construct_index_tree(mst, edge_trussness, triangle_trussness, 
            index_tree, index_hash);
    ASSERT_EQ(21, index_hash.size());
    ASSERT_EQ(4, index_tree.size());

    ASSERT_EQ(5, index_tree[index_hash[edge_composer(4, 1)]].k);
    ASSERT_EQ(10, index_tree[index_hash[edge_composer(4, 1)]].size);
    ASSERT_EQ(index_hash[edge_composer(2, 5)], 
            index_tree[index_hash[edge_composer(7, 8)]].parent);

    ASSERT_EQ(3, index_tree[index_hash[edge_composer(2, 1)]].k);
    ASSERT_EQ(21, index_tree[index_hash[edge_composer(2, 1)]].size);
    ASSERT_EQ(-1, index_tree[index_hash[edge_composer(2, 4)]].parent);

    ASSERT_EQ(4, index_tree[index_hash[edge_composer(5, 9)]].k);
    ASSERT_EQ(6, index_tree[index_hash[edge_composer(5, 9)]].size);
    ASSERT_EQ(index_hash[edge_composer(2, 0)], 
            index_tree[index_hash[edge_composer(5, 9)]].parent);

    inode_id_type virtual_root_id = index_tree[index_hash[edge_composer(2, 1)]].parent;
    ASSERT_EQ(2, index_tree[virtual_root_id].k);
    ASSERT_EQ(0, index_tree[virtual_root_id].size);
    ASSERT_EQ(-1, index_tree[virtual_root_id].parent);
}

/*
TEST(TCPIndexConstructionTest, ConstructionTest) {
    construct_tcp_index(net, edge_trussness, tcp_index);
    int exp_k[] = {3, 21, 9, 15, 29, 11, 8, 15, 15, 8};
    ASSERT_EQ(10, tcp_index.size());

    for (size_t i = 0; i < tcp_index.size(); i ++) {
        ASSERT_EQ(net->GetNI(i).GetDeg(), tcp_index[i].ego_graph->GetNodes());
        ASSERT_EQ(net->GetNI(i).GetDeg() - 1, tcp_index[i].ego_graph->GetEdges());
        ASSERT_EQ(net->GetNI(i).GetDeg() - 1, tcp_index[i].ego_triangle_trussness.size());
        int k = 0;
        for (eint_map::iterator iter = tcp_index[i].ego_triangle_trussness.begin();
            iter != tcp_index[i].ego_triangle_trussness.end();
            ++ iter) {
            k += iter->second;
        }
        ASSERT_EQ(exp_k[i], k);
    }
}
*/

TEST(QueryTest, RawTrussSESKQueryTest) {
    community_type truss_community;
    unordered_set<eid_type> visited_edges;
    vector< pair<eid_type, int> > queries;
    vector<int> exp_results;
    queries.push_back(make_pair(edge_composer(3, 7), 5));
    exp_results.push_back(10);
    queries.push_back(make_pair(edge_composer(4, 9), 4));
    exp_results.push_back(6);
    queries.push_back(make_pair(edge_composer(2, 4), 3));
    exp_results.push_back(21);

    for (size_t i = 0; i < queries.size(); i ++) {
        truss_community.clear();
        visited_edges.clear();
        truss_raw_edge_query(truss_community, 
                queries[i].first, queries[i].second, 
                net, edge_trussness, visited_edges);
        ASSERT_EQ(exp_results[i], truss_community.size());
    }
}

TEST(QueryTest, RawTrussSVSKQueryTest) {
    exact_qr_set_type truss_communities;
    vector< pair<vid_type, int> > queries;
    vector<qr_set_type> exp_community_infos;
    qr_set_type qr_set;
    qr_type qr;
    queries.push_back(make_pair(4, 4));
    qr_set.clear();
    qr.set(20, 6, 4);
    qr_set.push_back(qr);
    qr.set(4, 10, 5);
    qr_set.push_back(qr);
    exp_community_infos.push_back(qr_set);
    queries.push_back(make_pair(4, 3));
    qr_set.clear();
    qr.set(8, 21, 3);
    qr_set.push_back(qr);
    exp_community_infos.push_back(qr_set);
    queries.push_back(make_pair(1, 3));
    qr_set.clear();
    qr.set(8, 21, 3);
    qr_set.push_back(qr);
    exp_community_infos.push_back(qr_set);
    queries.push_back(make_pair(6, 5));
    qr_set.clear();
    exp_community_infos.push_back(qr_set);
    queries.push_back(make_pair(7, 2));
    qr_set.clear();
    qr.set(8, 21, 3);
    qr_set.push_back(qr);
    exp_community_infos.push_back(qr_set);

    for (size_t i = 0; i < queries.size(); i ++) {
        truss_communities.clear();
        truss_raw_query(truss_communities, 
                queries[i].first, queries[i].second, 
                net, edge_trussness);
        ASSERT_EQ(exp_community_infos[i].size(), truss_communities.size());
        for (size_t j = 0; j < truss_communities.size(); j ++) {
            bool found = false;
            for (qr_set_type::iterator 
                    iter = exp_community_infos[i].begin();
                    iter != exp_community_infos[i].end();
                    ++ iter) {
                if (std::find(truss_communities[j].begin(), 
                            truss_communities[j].end(),
                            vertex_extractor(edge_extractor(iter->iid))) !=
                        truss_communities[j].end()) {
                    found = true;
                    ASSERT_EQ(iter->size, truss_communities[j].size());
                    break;
                }
            }
            ASSERT_EQ(true, found);
        } 
    }
}

TEST(QueryTest, TrussSVSKQueryTest) {
    exact_qr_set_type truss_communities;
    qr_set_type truss_community_info;
    vector< pair<vid_type, int> > queries;
    queries.push_back(make_pair(4, 4));
    queries.push_back(make_pair(4, 3));
    queries.push_back(make_pair(1, 3));
    queries.push_back(make_pair(6, 5));
    queries.push_back(make_pair(7, 2));

    for (size_t i = 0; i < queries.size(); i ++) {
        truss_communities.clear();
        truss_community_info.clear();
        vector<vid_type> query_vids; 
        query_vids.push_back(queries[i].first);
        truss_k_query(truss_community_info, query_vids, queries[i].second,
                net, index_tree, index_hash);
        truss_raw_query(truss_communities, 
                queries[i].first, queries[i].second, 
                net, edge_trussness);
        ASSERT_EQ(truss_community_info.size(), truss_communities.size());
        for (size_t j = 0; j < truss_communities.size(); j ++) {
            bool found = false;
            for (qr_set_type::iterator 
                    iter = truss_community_info.begin();
                    iter != truss_community_info.end();
                    ++ iter) {
                if (std::find(truss_communities[j].begin(), 
                            truss_communities[j].end(),
                            vertex_extractor(edge_extractor(iter->iid))) !=
                        truss_communities[j].end()) {
                    found = true;
                    ASSERT_EQ(iter->size, truss_communities[j].size());
                    break;
                }
            }
            ASSERT_EQ(true, found);
        } 
    }
}

TEST(QueryTest, TrussMVMKQueryTest) {
    vector< vector<vid_type> > queries;
    // we dont have multiple community with same k in this graph.
    vector<vid_type> query;
    qr_set_type truss_community_info;
    vector<inode_id_type> exp_results;
    inode_id_type id1 = index_hash[edge_composer(2, 0)];
    inode_id_type id2 = index_hash[edge_composer(5, 9)];
    inode_id_type id3 = index_hash[edge_composer(7, 8)];
    query.clear();
    query.push_back(9);
    queries.push_back(query);
    exp_results.push_back(id2);
    query.clear();
    query.push_back(2);
    queries.push_back(query);
    exp_results.push_back(id1);
    query.clear();
    query.push_back(4);
    queries.push_back(query);
    exp_results.push_back(id3);
    query.clear();
    query.push_back(9);
    query.push_back(7);
    queries.push_back(query);
    exp_results.push_back(id1);
    query.clear();
    query.push_back(0);
    query.push_back(5);
    queries.push_back(query);
    exp_results.push_back(id1);
    query.clear();
    query.push_back(6);
    query.push_back(2);
    query.push_back(4);
    queries.push_back(query);
    exp_results.push_back(id1);
    query.clear();
    query.push_back(3);
    query.push_back(7);
    query.push_back(4);
    queries.push_back(query);
    exp_results.push_back(id3);
    query.clear();
    query.push_back(4);
    query.push_back(5);
    query.push_back(9);
    queries.push_back(query);
    exp_results.push_back(id2);

    for (size_t i = 0; i < queries.size(); i ++) {
        truss_community_info.clear();
        truss_maxk_query(truss_community_info, queries[i], 
                net, index_tree, index_hash);
        ASSERT_EQ(1, truss_community_info.size());
        ASSERT_EQ(exp_results[i], truss_community_info.begin()->iid);
    }
}

TEST(ArchiverTest, EdgeTrussnessSLTest) {
    eint_map stored_edge_trussness;
    counting_sorted_type stored_sorted_edge_trussness; 

    save_edge_trussness(edge_trussness, sorted_edge_trussness, 
            graph_fn, "checkpoint");
    load_edge_trussness(stored_edge_trussness, stored_sorted_edge_trussness, 
            graph_fn, "checkpoint");
    
    ASSERT_EQ(stored_edge_trussness, edge_trussness);
    ASSERT_EQ(6, stored_sorted_edge_trussness.size());
    size_t sorted_cnt = 0;
    for (size_t i = 0; i < stored_sorted_edge_trussness.size(); i ++) {
        sorted_cnt += stored_sorted_edge_trussness[i].size();
    }
    ASSERT_EQ(21, sorted_cnt);
}

TEST(ArchiverTest, MSTSLTest) {
    PUNGraph stored_mst;
    eint_map stored_triangle_trussness;
    unordered_map<eid_type, vid_type> stored_encode_table;
    unordered_map<vid_type, eid_type> stored_decode_table;

    save_mst(mst, triangle_trussness,
            encode_table, decode_table, graph_fn, "checkpoint");
    load_mst(stored_mst, stored_triangle_trussness,
            stored_encode_table, stored_decode_table, graph_fn, "checkpoint");

    ASSERT_EQ(mst->GetNodes(), stored_mst->GetNodes());
    ASSERT_EQ(mst->GetEdges(), stored_mst->GetEdges());
    ASSERT_EQ(triangle_trussness, stored_triangle_trussness);
    ASSERT_EQ(encode_table, stored_encode_table);
    ASSERT_EQ(decode_table, stored_decode_table);
    ASSERT_EQ(20, stored_triangle_trussness.size());
    ASSERT_EQ(21, stored_encode_table.size());
    ASSERT_EQ(21, stored_decode_table.size());
}

TEST(ArchiverTest, ITSLTest) {
    iidinode_map stored_index_tree;
    eiid_map stored_index_hash;

    save_index_tree(index_tree, index_hash, graph_fn, "checkpoint");
    load_index_tree(stored_index_tree, stored_index_hash, graph_fn, "checkpoint");

    ASSERT_EQ(index_tree, stored_index_tree);
    ASSERT_EQ(index_hash, stored_index_hash);
}

int main(int argc, char **argv) {
    net = TSnap::LoadEdgeList<PUNGraph>(graph_fn.c_str(), 0, 1);
    for (TUNGraph::TEdgeI EI = net->BegEI(); EI < net->EndEI(); EI++) 
        elist.insert(edge_composer(EI.GetSrcNId(), EI.GetDstNId()));
    create_checkpoint_dir("checkpoint");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
