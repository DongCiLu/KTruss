#include "common.hpp"

#ifndef EQUI_INDEX_HPP
#define EQUI_INDEX_HPP

inline void process_edge(eid_type e, int k,
        vid_type snID,
        queue<eid_type> &fifo,
        eint_map &edge_trussness,
        unordered_set<eid_type> &unvisited_edges,
        unordered_map<eid_type, unordered_set<vid_type>> &elist) {
    if (edge_trussness[e] == k) {
        if (unvisited_edges.find(e) != unvisited_edges.end()) {
            unvisited_edges.erase(e);
            fifo.push(e);
        }
    }
    else {
        if (elist[e].find(snID) == elist[e].end()) {
            elist[e].insert(snID);
        }
    }
}

void construct_equi_index(const PUNGraph &net,
        eint_map &edge_trussness,
        equi_hash_type &equi_hash,
        equi_index_type &equi_index) {
    // NOTE: run time for this algorithm is not optimal 
    // due to the use of SNAP graph.
    unordered_set<eid_type> unvisited_edges;
    unordered_map<eid_type, unordered_set<vid_type>> elist;
    unordered_map<int, unordered_set<eid_type>> tao;
    int max_k = 0;

    for (TUNGraph::TNodeI NI = net->BegNI(); NI < net->EndNI(); NI ++) {
        equi_hash[NI.GetId()] = unordered_set<vid_type>();
    }

    for (TUNGraph::TEdgeI EI = net->BegEI(); EI < net->EndEI(); EI ++) {
        eid_type e = edge_composer(EI.GetSrcNId(), EI.GetDstNId());
        unvisited_edges.insert(e);
        elist[e] = unordered_set<vid_type>();
        int k = edge_trussness[e];
        if (k > max_k) max_k = k;
        if (tao.find(k) == tao.end()) 
            tao[k] = unordered_set<eid_type>();
        tao[k].insert(e);
    }
    
    vid_type snID = 0;
    for (int k = 3; k <= max_k; k ++) {
        while(!tao[k].empty()) {
            eid_type e = *(tao[k].begin());
            EquiIndexNode equi(++snID, k);
            equi_index.super_graph->AddNode(equi.snID);
            equi_index.super_nodes.insert(make_pair(equi.snID, equi));
            queue<eid_type> fifo;
            unvisited_edges.erase(e);
            fifo.push(e);
            while(!fifo.empty()) {
                eid_type e = fifo.front();
                fifo.pop();
                pair<vid_type, vid_type> vpair = vertex_extractor(e);
                equi_index.super_nodes[equi.snID].edge_list.push_back(e);
                equi_hash[vpair.first].insert(equi.snID);
                equi_hash[vpair.second].insert(equi.snID);

                for (auto e_snID: elist[e]) {
                    equi_index.super_graph->AddEdge(e_snID, equi.snID);
                }
                vid_type low, high;
                get_low_high_deg_vertices(
                        net, vpair.first, vpair.second, low, high);
                for (int i = 0; i < net->GetNI(low).GetDeg(); i++) {
                    vid_type w = net->GetNI(low).GetNbrNId(i);
                    if (net->IsEdge(w, high)) {
                        eid_type e1 = edge_composer(low, w);
                        eid_type e2 = edge_composer(high, w);
                        if (edge_trussness[e1] >= k && edge_trussness[e2] >=k) {
                            process_edge(e1, k, equi.snID, fifo, 
                                    edge_trussness, unvisited_edges, elist);
                            process_edge(e2, k, equi.snID, fifo,
                                    edge_trussness, unvisited_edges, elist);
                        }
                    }
                }
                tao[k].erase(e);
                net->DelEdge(vpair.first, vpair.second);
            }
        }
    }
}

void generate_equi_index_from_existing_index(
        string graph_filename, 
        string checkpoint_dir) {
    Timer t;
    cout << "\n1. Loading graph and testcases" << endl;
    PUNGraph net = TSnap::LoadEdgeList<PUNGraph>(
            graph_filename.c_str(), 0, 1);
    t.print_n_update_timer();

    cout << "\n2. Loading indices" << endl;
    eint_map edge_trussness;
    counting_sorted_type sorted_edge_trussness; // not loaded
    iidinode_map index_tree;
    eiid_map index_hash;
    tcp_index_table_type tcp_index;
    equi_hash_type equi_hash;
    equi_index_type equi_index;
    load_edge_trussness(edge_trussness, sorted_edge_trussness,
        graph_filename, checkpoint_dir, false);
    t.print_n_update_timer();

    cout << "\n2. Construct equitruss index" << endl;
    // NOTE: net is destroyed after equi index construction
    construct_equi_index(net, edge_trussness, equi_hash, equi_index);
    t.print_n_update_timer();
    save_equi_index(equi_hash, equi_index, graph_filename, checkpoint_dir);
    t.update_timer();
}

#endif
