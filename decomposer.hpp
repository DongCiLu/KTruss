#include "common.hpp"

#ifndef DECOMPOSER_HPP
#define DECOMPOSER_HPP

// TODO: return max support value 
void compute_support(const PUNGraph &net, 
        unordered_set<eid_type> &elist,
        eint_map &edge_support,
        slow_sorted_type &sorted_edge_support) {
    for (TUNGraph::TEdgeI EI = net->BegEI(); EI < net->EndEI(); EI++) {
        int support = 0; 
        vid_type u = -1, v = -1;

        vid_type src = EI.GetSrcNId();
        vid_type dst = EI.GetDstNId();
        get_low_high_deg_vertices(net, src, dst, u, v);

        for (int i = 0; i < net->GetNI(u).GetDeg(); i++) {
            vid_type w = net->GetNI(u).GetNbrNId(i);
            // if (net->IsEdge(w, v))
            if (elist.find(edge_composer(w, v)) != elist.end())
                support += 1;
        }

        eid_type e = edge_composer(u, v);
        edge_support.insert(make_pair(e, support));
        sorted_edge_support.insert(make_pair(support, e));
    }
}

int compute_trussness(const PUNGraph &net,
        eint_map &edge_support,
        slow_sorted_type &sorted_edge_support, 
        eint_map &edge_trussness,
        counting_sorted_type &sorted_edge_trussness) {
    int k = 2;
    while (!sorted_edge_support.empty()) {
        while (!sorted_edge_support.empty() && 
                sorted_edge_support.begin()->first <= k - 2) {
            eid_type e = sorted_edge_support.begin()->second;
            pair<vid_type, vid_type> vpair = vertex_extractor(e);
            int support = sorted_edge_support.begin()->first;
            vid_type u = -1, v = -1;
            get_low_high_deg_vertices(net, vpair.first, vpair.second, u, v);
            for (int i = 0; i < net->GetNI(u).GetDeg(); i++) {
                vid_type w = net->GetNI(u).GetNbrNId(i);
                if (edge_support.find(edge_composer(w, v)) != 
                        edge_support.end() && 
                        edge_support.find(edge_composer(w, u)) != 
                        edge_support.end()) {
                    eid_type e_uw = edge_composer(u, w);
                    int uw_support = edge_support[e_uw];
                    sorted_edge_support.erase(make_pair(uw_support, e_uw));
                    edge_support[e_uw] -= 1;
                    uw_support -= 1;
                    sorted_edge_support.insert(make_pair(uw_support, e_uw));

                    eid_type e_vw = edge_composer(v, w);
                    int vw_support = edge_support[e_vw];
                    sorted_edge_support.erase(make_pair(vw_support, e_vw));
                    edge_support[e_vw] -= 1;
                    vw_support -= 1;
                    sorted_edge_support.insert(make_pair(vw_support, e_vw));
                }
            }
            edge_trussness[e] = k;
            // safe to cast, k is always positive
            size_t size_k = static_cast<size_t>(k); 
            if (size_k >= sorted_edge_trussness.size()) 
                sorted_edge_trussness.resize(k + 1, list<eid_type>());
            sorted_edge_trussness[k].push_back(e);
            edge_support.erase(e);
            sorted_edge_support.erase(make_pair(support, e));
        }
        k += 1;
    }
    return k - 1;
}

#endif
