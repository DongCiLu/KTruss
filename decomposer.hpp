#include "common.hpp"

#ifndef DECOMPOSER_HPP
#define DECOMPOSER_HPP

bool support_sort(const pair<eid_type, int> &a, 
        const pair<eid_type, int> &b) {
    return a.second < b.second;
}

// TODO: return max support value 
void compute_support(graph<vid_type> *net, 
        eint_map &edge_support,
        slow_sorted_type &sorted_edge_support) {
    size_t edge_cnt = 0;
    for (size_t i = 0; i < net->num_vertices(); i++) {
        for (size_t j = 0; j < net->out->num_vals_id(i); j ++) {
            edge_cnt ++;
            int support = 0; 
            vid_type u = -1, v = -1;
            vid_type src = i;
            vid_type dst = *(net->out->begin(i) + j);
            get_low_high_deg_vertices(net, src, dst, u, v);

            for (size_t k = 0; k < net->get_deg(u); k++) {
                vid_type w = net->get_nbr(u, k);
                if (net->elist.find(edge_composer(w, v)) != net->elist.end())
                    support += 1;
            }

            edge_support.insert(make_pair(edge_composer(u, v), support));
            sorted_edge_support.insert(make_pair(support, edge_composer(u, v)));
        }
    }
    cout << "edge count: " << edge_cnt << endl;
}

/*
int compute_trussness(graph<vid_type> *net,
        eint_map &edge_support,
        slow_sorted_type &sorted_edge_support, 
        eint_map &edge_trussness,
        counting_sorted_type &sorted_edge_trussness) {
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
*/

#endif
