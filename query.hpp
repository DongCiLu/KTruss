#include "common.hpp"
#include <algorithm>

#ifndef QUERY_HPP
#define QUERY_HPP


// this is not optimized
void truss_raw_edge_query(community_type &truss_community,
        eid_type query_e,
        int query_k, 
        PUNGraph &net,
        eint_map &edge_trussness,
        unordered_set<eid_type> &visited_edges) {
    // truss discovery for a single edge
    queue<eid_type> fifo;
    fifo.push(query_e);
    visited_edges.insert(query_e);
    while (!fifo.empty()) {
        eid_type e = fifo.front();
        fifo.pop();
        pair<vid_type, vid_type> vpair = vertex_extractor(e);
        truss_community.push_back(vpair);
        vid_type u = -1, v = -1;
        get_low_high_deg_vertices(net, vpair.first, vpair.second, u, v);
        for (int j = 0; j < net->GetNI(u).GetDeg(); j++) {
            vid_type w = net->GetNI(u).GetNbrNId(j);
            // test if it is a triangle
            if (net->IsEdge(v, w)) {
                eid_type adj_e1 = edge_composer(u, w);
                eid_type adj_e2 = edge_composer(v, w);
                if (edge_trussness[adj_e1] >= query_k &&
                        edge_trussness[adj_e2] >= query_k) {
                    if (visited_edges.find(adj_e1) == 
                            visited_edges.end()) {
                        fifo.push(adj_e1);
                        visited_edges.insert(adj_e1);
                    }
                    if (visited_edges.find(adj_e2) == 
                            visited_edges.end()) {
                        fifo.push(adj_e2);
                        visited_edges.insert(adj_e2);
                    }
                }
            }
        }
    }
}

void truss_raw_query(exact_qr_set_type &truss_communities,
        vid_type query_vid, 
        int query_k, 
        PUNGraph &net, 
        eint_map &edge_trussness) {
    // this is the k-truss query based on the edge trussness index
    unordered_set<eid_type> visited_edges;
    for (int i = 0; i < net->GetNI(query_vid).GetDeg(); i++) {
        vid_type u = net->GetNI(query_vid).GetNbrNId(i);
        eid_type e = edge_composer(u, query_vid);
        if (edge_trussness[e] >= query_k &&
                visited_edges.find(e) == visited_edges.end()) {
            community_type truss_community;
            truss_raw_edge_query(truss_community, e, query_k, 
                    net, edge_trussness, visited_edges);
            truss_communities.push_back(truss_community);
        }
    }
}

void branch_search(unordered_set<inode_id_type> &branch_nodes,
        inode_id_type iid, 
        iidinode_map &index_tree) {
    while(iid != -1) {
        branch_nodes.insert(iid);
        iid = index_tree[iid].parent;
    }
}

void truss_forest_single_v (
        unordered_set<inode_id_type> &single_forest,
        vid_type query_vid,
        PUNGraph &net,
        iidinode_map &index_tree,
        eiid_map &index_hash) {
    // find all the iids of the single vertex
    for (int i = 0; i < net->GetNI(query_vid).GetDeg(); i++) {
        vid_type v = net->GetNI(query_vid).GetNbrNId(i);
        eid_type e = edge_composer(query_vid, v);
        inode_id_type iid = index_hash[e];
        unordered_set<inode_id_type> branch_nodes;
        branch_search(branch_nodes, iid, index_tree);
        single_forest.insert(branch_nodes.begin(), branch_nodes.end());
    }
}

void truss_intersection_forest(
        unordered_set<inode_id_type> &intersection_forest,
        vector<vid_type>& query_vids, 
        PUNGraph &net,
        iidinode_map &index_tree,
        eiid_map &index_hash) {
    bool init = false;
    for (vector<vid_type>::const_iterator 
            citer = query_vids.begin();
            citer != query_vids.end();
            ++ citer) {
        unordered_set<inode_id_type> single_forest;
        vid_type query_vid = *citer;
        // find maxk community of a single vertex
        truss_forest_single_v(single_forest, query_vid,
                net, index_tree, index_hash);
        // store intersection of both forest
        if (init) {
            unordered_set<inode_id_type> new_intersection_forest;
            if (intersection_forest.size() < single_forest.size())
                intersection_forest.swap(single_forest);
            for (unordered_set<inode_id_type>::iterator 
                    iter = single_forest.begin();
                    iter != single_forest.end();
                    ++ iter) {
                if (intersection_forest.find(*iter) != 
                        intersection_forest.end())
                    new_intersection_forest.insert(*iter);
            }
            intersection_forest.swap(new_intersection_forest);
        }
        else {
            intersection_forest.swap(single_forest);
            init = true;
        }
    }
}
        
void truss_anyk_query(qr_set_type &res,
        vector<vid_type>& query_vids, 
        PUNGraph &net,
        iidinode_map &index_tree,
        eiid_map &index_hash) {
    // find intersection forest
    unordered_set<inode_id_type> intersection_forest;
    truss_intersection_forest(intersection_forest,
        query_vids, net, index_tree, index_hash);
    // output the intersection forest in required format
    for (unordered_set<inode_id_type>::iterator iter = 
            intersection_forest.begin();
            iter != intersection_forest.end();
            ++ iter) {
        qr_type res_node(*iter, index_tree[*iter].size, 
                index_tree[*iter].k);
        res.push_back(res_node);
    }
}

void truss_k_query(qr_set_type &res,
        vector<vid_type>& query_vids, 
        int query_k,
        PUNGraph &net,
        iidinode_map &index_tree,
        eiid_map &index_hash) {
    // find intersection forest
    unordered_set<inode_id_type> intersection_forest;
    truss_intersection_forest(intersection_forest,
        query_vids, net, index_tree, index_hash);
    // output the intersection forest in required format
    for (unordered_set<inode_id_type>::iterator 
            iter = intersection_forest.begin();
            iter != intersection_forest.end();
            ++ iter) {
        if (index_tree[*iter].k >= query_k) {
            inode_id_type piid = index_tree[*iter].parent;
            if (index_tree[piid].k >= query_k && 
                    intersection_forest.find(piid) != 
                    intersection_forest.end()) {
                continue;
            }
            else {
                qr_type res_node(*iter, index_tree[*iter].size, 
                        index_tree[*iter].k);
                res.push_back(res_node);
            }
        }
    }
}

void truss_maxk_query(qr_set_type &res,
        vector<vid_type>& query_vids, 
        PUNGraph &net,
        iidinode_map &index_tree,
        eiid_map &index_hash) {
    // find intersection forest
    unordered_set<inode_id_type> intersection_forest;
    truss_intersection_forest(intersection_forest,
        query_vids, net, index_tree, index_hash);
    // find truss communtiy with max k in the intersection forest
    int max_k = -1;
    for (unordered_set<inode_id_type>::iterator 
            iter = intersection_forest.begin();
            iter != intersection_forest.end();
            ++ iter) {
        if (index_tree[*iter].k > max_k) {
            max_k = index_tree[*iter].k;
            res.clear();
            qr_type res_node(*iter, index_tree[*iter].size, max_k);
            res.push_back(res_node);
        }
        else if (index_tree[*iter].k == max_k) {
            qr_type res_node(*iter, index_tree[*iter].size, max_k);
            res.push_back(res_node);
        }
    }
}

double truss_exact_query(exact_qr_set_type &res,
        qr_set_type &infos,
        PUNGraph mst,
        eint_map &triangle_trussness,
        unordered_map<inode_id_type, double> &exact_query_cache,
        bool cache_flag) {
    double total_time = 0;
    Timer t;
    for (size_t i = 0; i < infos.size(); i ++) {
        inode_id_type seed = infos[i].iid;
        size_t exp_size = infos[i].size;
        int k = infos[i].k;
        if (cache_flag && exact_query_cache.find(seed) != 
                exact_query_cache.end()) {
            total_time += exact_query_cache[seed];
            total_time += t.update_timer();
            continue;
        }
        community_type truss_community;
        unordered_set<vid_type> visited_vertices;
        queue<vid_type> fifo;
        fifo.push(seed);
        visited_vertices.insert(seed);
        while (!fifo.empty()) {
            vid_type u = fifo.front();
            pair<vid_type, vid_type> vpair = 
                vertex_extractor(edge_extractor(u));
            fifo.pop();
            truss_community.push_back(vpair);
            for (int j = 0; j < mst->GetNI(u).GetDeg(); j++) {
                vid_type v = mst->GetNI(u).GetNbrNId(j);
                if (visited_vertices.find(v) == 
                        visited_vertices.end()) {
                    eid_type e = edge_composer(u, v);
                    if (triangle_trussness[e] >= k) {
                        fifo.push(v);
                        visited_vertices.insert(v);
                    }
                }
            }
        }
        if (truss_community.size() != exp_size) {
            pair<vid_type, vid_type> vpair = 
                vertex_extractor(edge_extractor(seed));
            cout << "ERROR: expected community size not meet: " 
                << exp_size << " " << truss_community.size() 
                << " of community " << seed << "("
                << vpair.first << "," << vpair.second
                << ") with trussness " << k << endl;
        }
        res.push_back(truss_community);
        double time_diff = t.update_timer();
        if (cache_flag) {
            exact_query_cache[seed] = time_diff;
        }
        total_time += time_diff;
    }
    total_time += t.update_timer();
    return total_time;
}

void truss_path_query(vector<vid_type> &res,
        qr_set_type &infos,
        PUNGraph mst,
        eint_map &triangle_trussness,
        vid_type src, vid_type dst) {
    /*
     * We do not amid at finding the shotest maximin path,
     * So we only find path between src and dst on MST
     * in the first k-truss community if exists.
     */
    if (infos.empty()) return;
    vector<vid_type> src_seed_path, dst_seed_path;
    inode_id_type seed = infos[0].iid;
    size_t exp_size = infos[0].size;
    int k = infos[0].k;
    size_t real_size = 0;
    unordered_map<vid_type, vid_type> parent;
    queue<vid_type> fifo;
    fifo.push(seed);
    parent.insert(make_pair(seed, -1));
    while (!fifo.empty() && 
            (src_seed_path.empty() || dst_seed_path.empty())) {
        vid_type u = fifo.front();
        fifo.pop();
        real_size ++;
        
        if (src_seed_path.empty() && 
                is_vertex_in_mst_vertex(u, src)) {
            vid_type p = u;
            while(p != -1) {
                src_seed_path.push_back(p);
                p = parent[p];
            }
        }
        if (dst_seed_path.empty() && 
                is_vertex_in_mst_vertex(u, dst)) {
            vid_type p = u;
            while(p != -1) {
                dst_seed_path.push_back(p);
                p = parent[p];
            }
        }

        for (int j = 0; j < mst->GetNI(u).GetDeg(); j++) {
            vid_type v = mst->GetNI(u).GetNbrNId(j);
            if (parent.find(v) == parent.end()) {
                eid_type e = edge_composer(u, v);
                if (triangle_trussness[e] >= k) {
                    fifo.push(v);
                    parent.insert(make_pair(v, u));
                }
            }
        }
    }
    if (real_size > exp_size) {
        cout << "ERROR: real size and exp size not match: "
            << real_size << " " << exp_size << endl;
    }
    
    // combine both path at lca, root must be the same 
    // as long as they belong to the same truss community
    int lca_index = 0;
    while (src_seed_path[lca_index] == dst_seed_path[lca_index])
        lca_index ++;
    lca_index --;
    for (int i = src_seed_path.size() - 1; i >= lca_index; i--)
        res.push_back(src_seed_path[i]);
    for (int i = lca_index + 1; i < dst_seed_path.size(); i++)
        res.push_back(dst_seed_path[i]);
}

inline eid_type unordered_edge_composer(vid_type u, vid_type v) {
    return (eid_type(u) << SHIFTSIZE) + v;
}

void compute_vk(unordered_set<vid_type> &vk,
        vid_type seed,
        int query_k,
        PUNGraph ego_net,
        eint_map &ego_triangle_trussness) {
    queue<vid_type> fifo;
    unordered_set<vid_type> visited_vertices;
    fifo.push(seed);
    visited_vertices.insert(seed);
    while (!fifo.empty()) {
        vid_type u = fifo.front();
        fifo.pop();
        vk.insert(u);
        for (int j = 0; j < ego_net->GetNI(u).GetDeg(); j++) {
            vid_type v = ego_net->GetNI(u).GetNbrNId(j);
            if (visited_vertices.find(v) == visited_vertices.end() && 
                    ego_triangle_trussness[edge_composer(u, v)] >= query_k) {
                fifo.push(v);
                visited_vertices.insert(v);
            }
        }
    }
}

void truss_tcp_edge_query(community_type &truss_community,
        eid_type query_e,
        int query_k, 
        PUNGraph &net,
        tcp_index_table_type &tcp_index,
        unordered_set<eid_type> &visited_edges) {
    queue<eid_type> fifo;
    fifo.push(query_e);
    while (!fifo.empty()) {
        eid_type e = fifo.front();
        fifo.pop();
        if (visited_edges.find(e) == visited_edges.end()) {
            pair<vid_type, vid_type> vpair = vertex_extractor(e);
            unordered_set<vid_type> vk;
            if (tcp_index[vpair.first].ego_graph->IsNode(vpair.second)) {
                compute_vk(vk, vpair.second, query_k, 
                        tcp_index[vpair.first].ego_graph, 
                        tcp_index[vpair.first].ego_triangle_trussness);
            }
            for (unordered_set<vid_type>::iterator iter = vk.begin();
                    iter != vk.end();
                    ++ iter) {
                visited_edges.insert(unordered_edge_composer(vpair.first, *iter));
                if (visited_edges.find(unordered_edge_composer(*iter, vpair.first)) == 
                        visited_edges.end()) {
                    if (vpair.first < *iter)
                        truss_community.push_back(make_pair(vpair.first, *iter));
                    else
                        truss_community.push_back(make_pair(*iter, vpair.first));
                    fifo.push(unordered_edge_composer(*iter, vpair.first));
                }
            }
        }
    }
}

void truss_tcp_query(exact_qr_set_type &truss_communities,
        vid_type query_vid, 
        int query_k, 
        PUNGraph &net,
        eint_map &edge_trussness,
        tcp_index_table_type &tcp_index) {
    unordered_set<eid_type> visited_edges;
    for (int i = 0; i < net->GetNI(query_vid).GetDeg(); i++) {
        vid_type u = net->GetNI(query_vid).GetNbrNId(i);
        eid_type unordered_e = unordered_edge_composer(query_vid, u);
        eid_type e = edge_composer(u, query_vid);
        if (edge_trussness[e] >= query_k &&
                visited_edges.find(unordered_e) == visited_edges.end()) {
            community_type truss_community;
            truss_tcp_edge_query(truss_community, unordered_e, query_k, 
                    net, tcp_index, visited_edges);
            truss_communities.push_back(truss_community);
        }
    }
}

#endif
