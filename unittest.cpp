#include "gtest/gtest.h"
#include "common.hpp"
#include "testcase.hpp"
#include "decomposer.hpp"
#include "induced_graph.hpp"
#include "tree_index.hpp"

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
            net, edge_trussness, triangle_trussness, mst);
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

TEST(MSTConstructionTest, QueryTest) {
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
    ASSERT_EQ(-1, index_tree[virtual_root_id].k);
    ASSERT_EQ(0, index_tree[virtual_root_id].size);
    ASSERT_EQ(-1, index_tree[virtual_root_id].parent);
}

TEST(TreeIndexConstructionTest, QueryTest) {
}

int main(int argc, char **argv) {
    net = TSnap::LoadEdgeList<PUNGraph>(graph_fn.c_str(), 0, 1);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
