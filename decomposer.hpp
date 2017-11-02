#include "common.hpp"

#ifndef DECOMPOSER_HPP
#define DECOMPOSER_HPP

bool support_sort(const pair<eid_type, int> &a, 
        const pair<eid_type, int> &b) {
    return a.second < b.second;
}

// TODO: return max support value 
void compute_support(const PUNGraph &net, 
        eint_map &edge_support,
        set< pair<int, eid_type> > &sorted_edge_support) {
    for (TUNGraph::TEdgeI EI = net->BegEI(); EI < net->EndEI(); EI++) {
        int support = 0; 
        vid_type u = -1, v = -1;

        vid_type src = EI.GetSrcNId();
        vid_type dst = EI.GetDstNId();
        get_low_high_deg_vertices(net, src, dst, u, v);

        for (int i = 0; i < net->GetNI(u).GetDeg(); i++) {
            vid_type w = net->GetNI(u).GetNbrNId(i);
            if (net->IsEdge(w, v))
                support += 1;
        }

        edge_support.insert(make_pair(edge_composer(
                        EI.GetSrcNId(), EI.GetDstNId()), support));
        sorted_edge_support.insert(make_pair(support, edge_composer(
                        EI.GetSrcNId(), EI.GetDstNId())));
    }
}

int compute_trussness(const PUNGraph &net,
        eint_map &edge_support,
        set< pair<int, eid_type> > &sorted_edge_support, 
        eint_map &edge_trussness) {
    int k = 2;
    while (!sorted_edge_support.empty()) {
        while (!sorted_edge_support.empty() && 
                sorted_edge_support.begin()->first <= k - 2) {
            eid_type e = sorted_edge_support.begin()->second;
            int support = sorted_edge_support.begin()->first;
            vid_type u = -1, v = -1;
            get_low_high_deg_vertices(net, e.first, e.second, u, v);
            for (int i = 0; i < net->GetNI(u).GetDeg(); i++) {
                vid_type w = net->GetNI(u).GetNbrNId(i);
                if (edge_support.find(edge_composer(u, w)) != 
                        edge_support.end() &&
                        edge_support.find(edge_composer(w, v)) != 
                        edge_support.end()) {
                    int uw_support = edge_support[edge_composer(u, w)];
                    sorted_edge_support.erase(make_pair(
                                uw_support, edge_composer(u, w)));
                    edge_support[edge_composer(u, w)] -= 1;
                    uw_support -= 1;
                    sorted_edge_support.insert(make_pair(
                                uw_support, edge_composer(u, w)));
                    int vw_support = edge_support[edge_composer(v, w)];
                    sorted_edge_support.erase(make_pair(
                                vw_support, edge_composer(v, w)));
                    edge_support[edge_composer(v, w)] -= 1;
                    vw_support -= 1;
                    sorted_edge_support.insert(make_pair(
                                vw_support, edge_composer(v, w)));
                }
            }
            edge_trussness[e] = k;
            edge_support.erase(e);
            sorted_edge_support.erase(make_pair(support, e));
        }
        k += 1;
    }
    return k - 1;
}

#endif
