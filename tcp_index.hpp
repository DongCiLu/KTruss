#include "common.hpp"

#ifndef TCP_INDEX_HPP
#define TCP_INDEX_HPP

void generate_ego_mst_kruskal(tcp_index_type &tcpi,
        unordered_map<vid_type, vid_type> &cc,
        unordered_map<vid_type, int> &rank,
        vector< vector<eid_type> > &sorted_ego_triangle_trussness) {
    for (int edge_weight = sorted_ego_triangle_trussness.size() - 1; 
            edge_weight >= 0; edge_weight --) {
        for (size_t i = 0; i < sorted_ego_triangle_trussness[edge_weight].size(); i ++) {
            eid_type e = sorted_ego_triangle_trussness[edge_weight][i];
            vid_type u = e.first;
            vid_type v = e.second;
            vid_type pu = cc[u];
            vid_type pv = cc[v];

            while (pu != cc[pu]) 
                pu = cc[pu];
            while (pv != cc[pv]) 
                pv = cc[pv];

            if (pu != pv) {
                tcpi.ego_graph->AddEdge(u, v);
                tcpi.ego_triangle_trussness[edge_composer(u, v)] = edge_weight;
                if (rank[pu] > rank[pv])
                    cc[pv] = pu;
                else
                    cc[pu] = pv;

                if (rank[pu] == rank[pv])
                    rank[pv] ++;
            }
        }
    }
}

int get_max_ego_net_k(const PUNGraph &net,
        eint_map &edge_trussness,
        vid_type u) {
    int max_ego_net_k = 0;
    for (int i = 0; i < net->GetNI(u).GetDeg(); i++) {
        vid_type v = net->GetNI(u).GetNbrNId(i);
        if (edge_trussness[edge_composer(u,v)] > max_ego_net_k)
            max_ego_net_k = edge_trussness[edge_composer(u,v)];
    }
    return max_ego_net_k;
}

void construct_tcp_index_single(const PUNGraph &net,
        eint_map &edge_trussness,
        vid_type u,
        int max_ego_net_k, 
        tcp_index_type &tcpi) {
    unordered_map<vid_type, vid_type> cc;
    unordered_map<vid_type, int> rank;
    vector< vector<eid_type> > sorted_ego_triangle_trussness(
            max_ego_net_k + 1, vector<eid_type>());
    for (int i = 0; i < net->GetNI(u).GetDeg(); i++) {
        vid_type v = net->GetNI(u).GetNbrNId(i);
        cc.insert(make_pair(v, v));
        rank.insert(make_pair(v, 0));
        vid_type low, high;
        get_low_high_deg_vertices(net, u, v, low, high);
        for (int i = 0; i < net->GetNI(low).GetDeg(); i++) {
            vid_type w = net->GetNI(low).GetNbrNId(i);
            if (net->IsEdge(w, high)) {
                int uvw_trussness = std::min(
                        edge_trussness[edge_composer(u,v)],
                        edge_trussness[edge_composer(u,w)]);
                uvw_trussness = std::min(uvw_trussness, 
                        edge_trussness[edge_composer(v,w)]);
                sorted_ego_triangle_trussness[
                    uvw_trussness].push_back(edge_composer(v,w));
            }
        }
    }

    generate_ego_mst_kruskal(tcpi, cc, rank, sorted_ego_triangle_trussness);
}

void construct_tcp_index(const PUNGraph &net,
        eint_map &edge_trussness,
        tcp_index_table_type &tcp_index) {
    // initialize index with empty (no edge) ego graph for each vertex
    for (TUNGraph::TNodeI NI = net->BegNI(); NI < net->EndNI(); NI++) {
        tcp_index_type tcpi;
        tcpi.ego_graph = TUNGraph::New();
        vid_type u = NI.GetId();
        for (int i = 0; i < NI.GetDeg(); i++) {
            vid_type v = NI.GetNbrNId(i);
            tcpi.ego_graph->AddNode(v);
        }
        tcp_index[u] = tcpi;
    }

    // generate tcp index for each vertex 
    for (TUNGraph::TNodeI NI = net->BegNI(); NI < net->EndNI(); NI++) {
        vid_type u = NI.GetId();
        int max_ego_net_k = get_max_ego_net_k(net, edge_trussness, u);
        construct_tcp_index_single(
                net, edge_trussness, u, max_ego_net_k, tcp_index[u]);
    }
}

#endif
