#include "common.hpp"
#include "tree_index.hpp"

#ifndef INDUCED_GRAPH_HPP
#define INDUCED_GRAPH_HPP

/* 
 * In this new graph, edges become vertex.
 * triangles become edges,
 * vertex weight is edge trussness
 * edge weight is triangle trussness
 */
int construct_mst(const PUNGraph &net,
        eint_map &edge_trussness,
        eint_map &triangle_trussness,
        PUNGraph &mst) {
    unordered_map<vid_type, vid_type> cc;
    unordered_map<vid_type, int> rank;
    set< pair<int, eid_type> > sorted_triangle_trussness;

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
                triangle_trussness[e1] = uvw_trussness;
                triangle_trussness[e2] = uvw_trussness;
                triangle_trussness[e3] = uvw_trussness;
                sorted_triangle_trussness.insert(
                        make_pair(uvw_trussness, e1));
                sorted_triangle_trussness.insert(
                        make_pair(uvw_trussness, e2));
                sorted_triangle_trussness.insert(
                        make_pair(uvw_trussness, e3));
            }
        }
    }

    // Kruskal's algorithm
    for (typename set< pair<int, eid_type> >::reverse_iterator
            riter = sorted_triangle_trussness.rbegin();
            riter != sorted_triangle_trussness.rend();
            ++ riter) {
        vid_type x = riter->second.first;
        vid_type y = riter->second.second;
        vid_type px = cc[x];
        vid_type py = cc[y];
        int trussness = riter->first;

        while (px != cc[px]) 
            px = cc[px];
        while (py != cc[py]) 
            py = cc[py];

        if (px != py) {
            mst->AddEdge(riter->second.first, riter->second.second);
            if (rank[px] > rank[py])
                cc[py] = px;
            else
                cc[px] = py;

            if (rank[px] == rank[py])
                rank[py] ++;
        }
        px += trussness; // i forgot what is this for.
    }

    // count how many connected components are there
    set<int> cc_heads;
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

#endif
