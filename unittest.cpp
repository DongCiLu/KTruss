#include "gtest/gtest.h"
#include "common.hpp"
#include "testcase.hpp"
#include "decomposer.hpp"
#include "induced_graph.hpp"
#include "tree_index.hpp"
#include "tcp_index.hpp"
#include "query.hpp"

// TODO: design a new example
 
string graph_fn = "datasets/testgraph_truss.txt"; // contains 10 vertices and 21 edges
string testcase_fn = "datasets/testcases_truss/unittest.txt";
vector<vid_type> testcases;
size_t num_testcases = 0;

PUNGraph net;

eint_map edge_support;
set< pair<int, eid_type> > sorted_edge_support; 
eint_map edge_trussness;

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
    compute_support(net, edge_support, sorted_edge_support);
    eint_map exp_edge_support;
    set< pair<int, eid_type> > exp_sorted_edge_support; 

    exp_edge_support.insert(make_pair(edge_composer(0, 1), 1));
    exp_sorted_edge_support.insert(make_pair(1, edge_composer(0, 1)));
    exp_edge_support.insert(make_pair(edge_composer(0, 2), 1));
    exp_sorted_edge_support.insert(make_pair(1, edge_composer(0, 2)));
    exp_edge_support.insert(make_pair(edge_composer(1, 2), 2));
    exp_sorted_edge_support.insert(make_pair(2, edge_composer(1, 2)));
    exp_edge_support.insert(make_pair(edge_composer(1, 4), 4));
    exp_sorted_edge_support.insert(make_pair(4, edge_composer(1, 4)));
    exp_edge_support.insert(make_pair(edge_composer(1, 3), 3));
    exp_sorted_edge_support.insert(make_pair(3, edge_composer(1, 3)));
    exp_edge_support.insert(make_pair(edge_composer(3, 4), 3));
    exp_sorted_edge_support.insert(make_pair(3, edge_composer(3, 4)));
    exp_edge_support.insert(make_pair(edge_composer(1, 8), 3));
    exp_sorted_edge_support.insert(make_pair(3, edge_composer(1, 8)));
    exp_edge_support.insert(make_pair(edge_composer(3, 8), 3));
    exp_sorted_edge_support.insert(make_pair(3, edge_composer(3, 8)));
    exp_edge_support.insert(make_pair(edge_composer(4, 8), 3));
    exp_sorted_edge_support.insert(make_pair(3, edge_composer(4, 8)));
    exp_edge_support.insert(make_pair(edge_composer(1, 7), 3));
    exp_sorted_edge_support.insert(make_pair(3, edge_composer(1, 7)));
    exp_edge_support.insert(make_pair(edge_composer(3, 7), 3));
    exp_sorted_edge_support.insert(make_pair(3, edge_composer(3, 7)));
    exp_edge_support.insert(make_pair(edge_composer(7, 8), 3));
    exp_sorted_edge_support.insert(make_pair(3, edge_composer(7, 8)));
    exp_edge_support.insert(make_pair(edge_composer(4, 7), 3));
    exp_sorted_edge_support.insert(make_pair(3, edge_composer(4, 7)));
    exp_edge_support.insert(make_pair(edge_composer(2, 4), 2));
    exp_sorted_edge_support.insert(make_pair(2, edge_composer(2, 4)));
    exp_edge_support.insert(make_pair(edge_composer(2, 5), 1));
    exp_sorted_edge_support.insert(make_pair(1, edge_composer(2, 5)));
    exp_edge_support.insert(make_pair(edge_composer(4, 5), 3));
    exp_sorted_edge_support.insert(make_pair(3, edge_composer(4, 5)));
    exp_edge_support.insert(make_pair(edge_composer(4, 6), 2));
    exp_sorted_edge_support.insert(make_pair(2, edge_composer(4, 6)));
    exp_edge_support.insert(make_pair(edge_composer(5, 6), 2));
    exp_sorted_edge_support.insert(make_pair(2, edge_composer(5, 6)));
    exp_edge_support.insert(make_pair(edge_composer(9, 6), 2));
    exp_sorted_edge_support.insert(make_pair(2, edge_composer(9, 6)));
    exp_edge_support.insert(make_pair(edge_composer(4, 9), 2));
    exp_sorted_edge_support.insert(make_pair(2, edge_composer(4, 9)));
    exp_edge_support.insert(make_pair(edge_composer(5, 9), 2));
    exp_sorted_edge_support.insert(make_pair(2, edge_composer(5, 9)));

    ASSERT_EQ(exp_edge_support, edge_support);
    ASSERT_EQ(exp_sorted_edge_support, sorted_edge_support);
}

TEST(DecomposerTest, EdgeTrussnessTest) {
    max_net_k = compute_trussness(
            net, edge_support, sorted_edge_support, edge_trussness);
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
}

TEST(MSTConstructionTest, ConstructionTest) {
    mst = TUNGraph::New();
    int num_cc = construct_mst(
            net, edge_trussness, triangle_trussness, mst, max_net_k);
    ASSERT_EQ(mst->GetNodes(), net->GetEdges());
    ASSERT_EQ(mst->GetEdges(), mst->GetNodes() - num_cc);
    ASSERT_EQ(1, num_cc);
    ASSERT_EQ(21, mst->GetNodes());
    ASSERT_EQ(51, triangle_trussness.size());

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

TEST(TCPIndexConstructionTest, ConstructionTest) {
    construct_tcp_index(net, edge_trussness, tcp_index);
    ASSERT_EQ(10, tcp_index.size());

    ASSERT_EQ(2, tcp_index[0].ego_graph->GetNodes());
    ASSERT_EQ(8, tcp_index[4].ego_graph->GetNodes());
    ASSERT_EQ(3, tcp_index[9].ego_graph->GetNodes());
    ASSERT_EQ(4, tcp_index[3].ego_graph->GetNodes());
}

TEST(QueryTest, RawTrussSESKQueryTest) {
    community_type truss_community;
    unordered_set<eid_type, boost::hash<eid_type> > visited_edges;
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
    qr_set.insert(qr);
    qr.set(4, 10, 5);
    qr_set.insert(qr);
    exp_community_infos.push_back(qr_set);
    queries.push_back(make_pair(4, 3));
    qr_set.clear();
    qr.set(8, 21, 3);
    qr_set.insert(qr);
    exp_community_infos.push_back(qr_set);
    queries.push_back(make_pair(1, 3));
    qr_set.clear();
    qr.set(8, 21, 3);
    qr_set.insert(qr);
    exp_community_infos.push_back(qr_set);
    queries.push_back(make_pair(6, 5));
    qr_set.clear();
    exp_community_infos.push_back(qr_set);
    queries.push_back(make_pair(7, 2));
    qr_set.clear();
    qr.set(8, 21, 3);
    qr_set.insert(qr);
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
                if (truss_communities[j].find(edge_extractor(iter->iid)) != 
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
                if (truss_communities[j].find(edge_extractor(iter->iid)) != 
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
    query.clear();
    query.push_back(9);
    queries.push_back(query);
    exp_results.push_back(20);
    query.clear();
    query.push_back(2);
    queries.push_back(query);
    exp_results.push_back(8);
    query.clear();
    query.push_back(4);
    queries.push_back(query);
    exp_results.push_back(4);
    query.clear();
    query.push_back(9);
    query.push_back(7);
    queries.push_back(query);
    exp_results.push_back(8);
    query.clear();
    query.push_back(0);
    query.push_back(5);
    queries.push_back(query);
    exp_results.push_back(8);
    query.clear();
    query.push_back(6);
    query.push_back(2);
    query.push_back(4);
    queries.push_back(query);
    exp_results.push_back(8);
    query.clear();
    query.push_back(3);
    query.push_back(7);
    query.push_back(4);
    queries.push_back(query);
    exp_results.push_back(4);
    query.clear();
    query.push_back(4);
    query.push_back(5);
    query.push_back(9);
    queries.push_back(query);
    exp_results.push_back(20);

    for (size_t i = 0; i < queries.size(); i ++) {
        truss_community_info.clear();
        truss_maxk_query(truss_community_info, queries[i], 
                net, index_tree, index_hash);
        ASSERT_EQ(1, truss_community_info.size());
        ASSERT_EQ(exp_results[i], truss_community_info.begin()->iid);
    }
}

int main(int argc, char **argv) {
    net = TSnap::LoadEdgeList<PUNGraph>(graph_fn.c_str(), 0, 1);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
