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
        pair<vid_type, vid_type> vpair = vertex_extractor(e);
        fifo.pop();
        truss_community.push_back(vpair);
        for (int j = 0; j < net->GetNI(vpair.first).GetDeg(); j++) {
            vid_type w = net->GetNI(vpair.first).GetNbrNId(j);
            // test if it is a triangle
            if (net->IsEdge(vpair.second, w)) {
                eid_type adj_e1 = edge_composer(vpair.first, w);
                eid_type adj_e2 = edge_composer(vpair.second, w);
                if (edge_trussness[adj_e1] >= query_k &&
                        edge_trussness[adj_e2] >= query_k) {
                    if (visited_edges.find(adj_e1) == visited_edges.end()) {
                        fifo.push(adj_e1);
                        visited_edges.insert(adj_e1);
                    }
                    if (visited_edges.find(adj_e2) == visited_edges.end()) {
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

void truss_forest_single_v (unordered_set<inode_id_type> &single_forest,
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

void truss_intersection_forest(unordered_set<inode_id_type> &intersection_forest,
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
                if (intersection_forest.find(*iter) != intersection_forest.end())
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
    for (unordered_set<inode_id_type>::iterator iter = intersection_forest.begin();
            iter != intersection_forest.end();
            ++ iter) {
        qr_type res_node(*iter, index_tree[*iter].size, index_tree[*iter].k);
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
    for (unordered_set<inode_id_type>::iterator iter = intersection_forest.begin();
            iter != intersection_forest.end();
            ++ iter) {
        if (index_tree[*iter].k >= query_k) {
            inode_id_type piid = index_tree[*iter].parent;
            if (index_tree[piid].k >= query_k && 
                    intersection_forest.find(piid) != intersection_forest.end()) {
                continue;
            }
            else {
                qr_type res_node(*iter, index_tree[*iter].size, index_tree[*iter].k);
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
    for (unordered_set<inode_id_type>::iterator iter = intersection_forest.begin();
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

void truss_exact_query(exact_qr_set_type &res,
        qr_set_type &infos,
        PUNGraph mst,
        eint_map &triangle_trussness) {
    for (size_t i = 0; i < infos.size(); i ++) {
        inode_id_type seed = infos[i].iid;
        size_t exp_size = infos[i].size;
        int k = infos[i].k;

        community_type truss_community;
        unordered_set<vid_type> visited_vertices;
        queue<vid_type> fifo;
        fifo.push(seed);
        visited_vertices.insert(seed);
        while (!fifo.empty()) {
            vid_type u = fifo.front();
            pair<vid_type, vid_type> vpair = vertex_extractor(edge_extractor(u));
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
    }
}

#endif
