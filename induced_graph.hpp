#include "common.hpp"

#ifndef INDUCED_GRAPH_HPP
#define INDUCED_GRAPH_HPP

/* 
 * In this new graph, edges become vertex.
 * triangles become edges,
 * vertex weight is edge trussness
 * edge weight is triangle trussness
 * ASSUME MAX_NET_K << NUMBER OF TRIANGLES IN THE GRAPH
 * We use counting sort
 */

void test_and_add_mst_edge(vid_type u, vid_type v,
        int uvw_trussness,
        unordered_map<vid_type, vid_type> &cc,
        unordered_map<vid_type, int> &rank,
        PUNGraph &mst,
        eint_map &triangle_trussness) {
    vid_type pu = cc[u];
    vid_type pv = cc[v];

    while (pu != cc[pu]) 
        pu = cc[pu];
    while (pv != cc[pv]) 
        pv = cc[pv];

    if (pu != pv) {
        mst->AddEdge(u, v);
        triangle_trussness[edge_composer(u, v)] = uvw_trussness;
        if (rank[pu] > rank[pv])
            cc[pv] = pu;
        else
            cc[pu] = pv;

        if (rank[pu] == rank[pv])
            rank[pv] ++;
    }
}

size_t count_cc(unordered_map<vid_type, vid_type> &cc) {
    unordered_set<int> cc_heads;
    for (unordered_map<vid_type, vid_type>::iterator iter = cc.begin();
            iter != cc.end();
            ++ iter) {
        vid_type u = iter->second;
        while (u != cc[u])
            u = cc[u];
        cc_heads.insert(u);
    }

    return cc_heads.size();
}

size_t construct_mst(graph<vid_type> *net,
        eint_map &edge_trussness,
        counting_sorted_type &sorted_edge_trussness,
        PUNGraph &mst,
        eint_map &triangle_trussness) {

    unordered_map<vid_type, vid_type> cc;
    unordered_map<vid_type, int> rank;

    for (TUNGraph::TEdgeI EI = net->BegEI(); EI < net->EndEI(); EI++) {
        vid_type v = mst_vid_composer(EI.GetSrcNId(), EI.GetDstNId());
        cc.insert(make_pair(v, v));
        rank.insert(make_pair(v, 0));
        mst->AddNode(v);
    }
    cout << "cc size: " << cc.size() << " * " <<  2 * 4 << endl;
    cout << "rank size: " << cc.size() << " * " <<  2 * 4 << endl;

    size_t triangle_cnt = 0, triangle_access_cnt = 0;
    for (int edge_weight = sorted_edge_trussness.size() - 1; 
            edge_weight >= 0; edge_weight --) {
        for (list<eid_type>::iterator 
                iter = sorted_edge_trussness[edge_weight].begin();
                iter != sorted_edge_trussness[edge_weight].end();
                ++ iter) {
            vid_type u = -1, v = -1;
            get_low_high_deg_vertices(net, iter->first, iter->second, u, v);
            int u_deg = net->GetNI(u).GetDeg();
            for (int i = 0; i < u_deg; i++) {
                vid_type w = net->GetNI(u).GetNbrNId(i);
                if (net->IsEdge(w, v)) { 
                    triangle_access_cnt ++;
                    eid_type e1 = edge_composer(u, v);
                    eid_type e2 = edge_composer(u, w);
                    eid_type e3 = edge_composer(v, w);
                    pair<int, eid_type> e1_tuple(edge_trussness[e1], e1);
                    pair<int, eid_type> e2_tuple(edge_trussness[e2], e2);
                    pair<int, eid_type> e3_tuple(edge_trussness[e3], e3);
                    // make sure each triangle is only processed for once
                    if (e1_tuple < e2_tuple && e1_tuple < e3_tuple) {
                        triangle_cnt ++;
                        int uvw_trussness = edge_trussness[e1];
                        vid_type v1 = mst_vid_composer(e1.first, e1.second);
                        vid_type v2 = mst_vid_composer(e2.first, e2.second);
                        vid_type v3 = mst_vid_composer(e3.first, e3.second);
                        test_and_add_mst_edge(v1, v2, uvw_trussness, 
                                cc, rank, mst, triangle_trussness);
                        test_and_add_mst_edge(v1, v3, uvw_trussness, 
                                cc, rank, mst, triangle_trussness);
                        // we do not need to add all three edges
                        // test_and_add_mst_edge(v2, v3, uvw_trussness, 
                                // cc, rank, mst, triangle_trussness);
                    }
                }
            }
        }
    }
    cout << "triangle count: " << triangle_cnt 
        << " triangle access count: " << triangle_access_cnt << endl;
    return count_cc(cc);
}

#endif
