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

void discover_and_sort_triangles(const PUNGraph &net,
        eint_map &edge_trussness,
        PUNGraph &mst,
        int max_net_k, 
        unordered_map<vid_type, vid_type> &cc,
        unordered_map<vid_type, int> &rank,
        vector< vector<eid_type> > &sorted_triangle_trussness) {
    // create set for each mst vertex and sort mst edges by weight
    for (TUNGraph::TEdgeI EI = net->BegEI(); EI < net->EndEI(); EI++) {
        vid_type u = -1, v = -1;
        vid_type src = EI.GetSrcNId();
        vid_type dst = EI.GetDstNId();
        get_low_high_deg_vertices(net, src, dst, u, v);
        int u_deg = net->GetNI(u).GetDeg();

        // if not connected with any triangle
        vid_type v1 = mst_vid_composer(u,v);
        if (cc.find(v1) == cc.end()) {
            cc.insert(make_pair(v1, v1));
            rank.insert(make_pair(v1, 0));
            mst->AddNode(v1);
        }
        for (int i = 0; i < u_deg; i++) {
            vid_type w = net->GetNI(u).GetNbrNId(i);
            int uvw_trussness = 0;
            // ensure each triangle only inserted once. i
            // only add when u is the smallest vid in uvw
            if (w < u && w < v && net->IsEdge(w, v)) { 
                uvw_trussness = std::min(
                        edge_trussness[edge_composer(u,v)],
                        edge_trussness[edge_composer(u,w)]);
                uvw_trussness = std::min(uvw_trussness, 
                        edge_trussness[edge_composer(v,w)]);
                // create set for each vertex for later mst algorithm
                vid_type v2 = mst_vid_composer(u,w);
                vid_type v3 = mst_vid_composer(w,v);
                if (cc.find(v2) == cc.end()) {
                    cc.insert(make_pair(v2, v2));
                    rank.insert(make_pair(v2, 0));
                    mst->AddNode(v2);
                }
                if (cc.find(v3) == cc.end()) {
                    cc.insert(make_pair(v3, v3));
                    rank.insert(make_pair(v3, 0));
                    mst->AddNode(v3);
                }
                // create and sort edges
                eid_type e1 = edge_composer(v1, v2);
                eid_type e2 = edge_composer(v2, v3);
                eid_type e3 = edge_composer(v1, v3);
                /*
                triangle_trussness[e1] = uvw_trussness;
                triangle_trussness[e2] = uvw_trussness;
                triangle_trussness[e3] = uvw_trussness;
                */

                sorted_triangle_trussness[uvw_trussness].push_back(e1);
                sorted_triangle_trussness[uvw_trussness].push_back(e2);
                sorted_triangle_trussness[uvw_trussness].push_back(e3);

                /*
                sorted_triangle_trussness.insert(
                        make_pair(uvw_trussness, e1));
                sorted_triangle_trussness.insert(
                        make_pair(uvw_trussness, e2));
                sorted_triangle_trussness.insert(
                        make_pair(uvw_trussness, e3));
                        */
            }
        }
    }
    cout << "cc size: " << cc.size() << " * " <<  2 * 4 << endl;
    cout << "rank size: " << cc.size() << " * " <<  2 * 4 << endl;
    size_t sorted_triangle_size = 0;
    for (size_t i = 0; i < sorted_triangle_trussness.size(); i ++) {
        sorted_triangle_size += sorted_triangle_trussness[i].size();
    }
    cout << "sorted triangle size: " << sorted_triangle_size << " * " <<  2 * 4 << endl;
}

void generate_mst_kruskal(PUNGraph &mst,
        unordered_map<vid_type, vid_type> &cc,
        unordered_map<vid_type, int> &rank,
        eint_map &triangle_trussness,
        vector< vector<eid_type> > &sorted_triangle_trussness) {
    for (int edge_weight = sorted_triangle_trussness.size() - 1; 
            edge_weight >= 0; edge_weight --) {
        for (size_t i = 0; i < sorted_triangle_trussness[edge_weight].size(); i ++) {
            eid_type e = sorted_triangle_trussness[edge_weight][i];
            vid_type u = e.first;
            vid_type v = e.second;
            vid_type pu = cc[u];
            vid_type pv = cc[v];

            while (pu != cc[pu]) 
                pu = cc[pu];
            while (pv != cc[pv]) 
                pv = cc[pv];

            if (pu != pv) {
                mst->AddEdge(u, v);
                triangle_trussness[e] = edge_weight;
                if (rank[pu] > rank[pv])
                    cc[pv] = pu;
                else
                    cc[pu] = pv;

                if (rank[pu] == rank[pv])
                    rank[pv] ++;
            }
            pu += edge_weight; // I forgot what is this for.
        }
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

size_t construct_mst(const PUNGraph &net,
        eint_map &edge_trussness,
        eint_map &triangle_trussness,
        PUNGraph &mst,
        int max_net_k) {
    unordered_map<vid_type, vid_type> cc;
    unordered_map<vid_type, int> rank;
    vector< vector<eid_type> > sorted_triangle_trussness(
            max_net_k + 1, vector<eid_type>());

    discover_and_sort_triangles(net, edge_trussness, 
            mst, max_net_k, cc, rank, sorted_triangle_trussness);

    generate_mst_kruskal(mst, cc, rank, 
            triangle_trussness, sorted_triangle_trussness);

    return count_cc(cc);
}

#endif
