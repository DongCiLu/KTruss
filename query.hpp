#include "common.hpp"

#ifndef QUERY_HPP
#define QUERY_HPP

struct QR{
    inode_id_type inode_index;
    int size;
    int k;

    QR(inode_id_type inode_index = -1, int size = -1, int k = -1): 
        inode_index(inode_index), size(size), k(k) { }

    bool operator==(const QR& rhs) const {
        return this->inode_index == rhs.inode_index;
    }
};

struct QR_hash {
    size_t operator()(const QR& _qr) const {
        return std::hash<inode_id_type>()(_qr.inode_index);
    }
};

vector< set<eid_type> > truss_raw_query(
        int query_k, vid_type query_vid, 
        const PUNGraph &net, 
        eint_map &edge_trussness) {
    // this is the k-truss query based on the edge trussness index
    vector< set<eid_type> > truss_communities;
    set<eid_type> visited_edges;
    for (int i = 0; i < net->GetNI(query_vid).GetDeg(); i++) {
        vid_type u = net->GetNI(query_vid).GetNbrNId(i);
        if (edge_trussness[edge_composer(u, query_vid)] >= query_k &&
                visited_edges.find(edge_composer(u, query_vid)) == visited_edges.end()) {
            set<eid_type> truss_community;
            queue<eid_type> fifo;
            fifo.push(edge_composer(u, query_vid));
            visited_edges.insert(edge_composer(u, query_vid));
            while (!fifo.empty()) {
                eid_type e = fifo.front();
                fifo.pop();
                truss_community.insert(e);
                for (int j = 0; j < net->GetNI(e.first).GetDeg(); j++) {
                    vid_type w = net->GetNI(e.first).GetNbrNId(j);
                    if (net->IsEdge(e.second, w)) {
                        if (edge_trussness[edge_composer(e.first, w)] >= query_k &&
                                edge_trussness[edge_composer(e.second, w)] >= query_k) {
                            if (visited_edges.find(edge_composer(e.first, w)) ==
                                    visited_edges.end()) {
                                fifo.push(edge_composer(e.first, w));
                                visited_edges.insert(edge_composer(e.first, w));
                            }
                            if (visited_edges.find(edge_composer(e.second, w)) ==
                                    visited_edges.end()) {
                                fifo.push(edge_composer(e.second, w));
                                visited_edges.insert(edge_composer(e.second, w));
                            }
                        }
                    }
                }
            }
            truss_communities.push_back(truss_community);
        }
    }

    return truss_communities;
}

vector<inode_id_type> branch_search(inode_id_type& iid, 
        iidinode_map &index_tree,
        int query_k = -1) {
    // use vector because in some query sequence matters
    vector<inode_id_type> branch_nodes;
    while(iid != -1 && index_tree[iid].k >= query_k) {
        branch_nodes.push_back(iid);
        iid = index_tree[iid].parent;
    }
    return branch_nodes;
}

unordered_set<QR, QR_hash> truss_maxk_query(vector<vid_type>& query_vids, 
        PUNGraph &net,
        iidinode_map &index_tree,
        eiid_map &index_hash) {
    unordered_set<QR, QR_hash> res;
    for (vector<vid_type>::const_iterator 
            citer = query_vids.begin();
            citer != query_vids.end();
            ++ citer) {
        int max_k = -1;
        unordered_set<QR, QR_hash> single_res;
        vid_type u = *citer;
        int u_deg = net->GetNI(u).GetDeg();
        for (int i = 0; i < u_deg; i++) {
            vid_type v = net->GetNI(u).GetNbrNId(i);
            inode_id_type iid = index_hash[edge_composer(u, v)];
            if (index_tree[iid].k > max_k){
                max_k = index_tree[iid].k;
                single_res.clear();
                QR res_node(iid, index_tree[iid].size, index_tree[iid].k);
                single_res.insert(res_node);
            }
            else if (index_tree[iid].k == max_k){
                QR res_node(iid, index_tree[iid].size, index_tree[iid].k);
                single_res.insert(res_node);
            }
            else {
                continue;
            }
        }

        if (res.empty()) {
            res = single_res;
        }
        else {
            unordered_set<QR, QR_hash> temp_res = res;
            res.clear();
            int max_k = -1;
            for (unordered_set<QR, QR_hash>::iterator titer = temp_res.begin();
                    titer != temp_res.end();
                    ++ titer) {
                for (unordered_set<QR, QR_hash>::iterator siter = single_res.begin();
                        siter != single_res.end();
                        ++ siter) {
                    inode_id_type a = titer->inode_index;
                    inode_id_type b = siter->inode_index;
                    while (a != -1 && b != -1){
                        if (index_tree[a].k > index_tree[b].k) {
                            a = index_tree[a].parent;
                        }
                        else if (index_tree[a].k < index_tree[b].k) {
                            b = index_tree[b].parent;
                        }
                        else {
                            if (a == b) {
                                // found a common ancestor
                                if (index_tree[a].k > max_k) {
                                    max_k = index_tree[a].k;
                                    res.clear();
                                    QR res_node(a, index_tree[a].size, index_tree[a].k);
                                    res.insert(res_node);
                                }
                                else if (index_tree[a].k == max_k) {
                                    QR res_node(a, index_tree[a].size, index_tree[a].k);
                                    res.insert(res_node);
                                }
                                else {
                                    break;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return res;
}

unordered_set<QR, QR_hash> truss_k_query(vector<vid_type>& query_vids, 
        int query_k,
        PUNGraph &net,
        iidinode_map &index_tree,
        eiid_map &index_hash) {
    unordered_set<QR, QR_hash> res;
    for (vector<vid_type>::const_iterator 
            citer = query_vids.begin();
            citer != query_vids.end();
            ++ citer) {
        vid_type u = *citer;
        int u_deg = net->GetNI(u).GetDeg();
        for (int i = 0; i < u_deg; i++) {
            vid_type v = net->GetNI(u).GetNbrNId(i);
            inode_id_type iid = index_hash[edge_composer(u, v)];
            vector<inode_id_type> branch_nodes = branch_search(iid, index_tree, query_k); 
            inode_id_type last_iid = -1;
            if (!branch_nodes.empty())
                last_iid = branch_nodes[-1];
            if (last_iid != -1 && index_tree[last_iid].k == query_k) {
                QR res_node(last_iid, index_tree[last_iid].size, index_tree[last_iid].k);
                res.insert(res_node);
            }
        }
    }
    return res;
}

unordered_set<QR, QR_hash> truss_anyk_query(vector<vid_type>& query_vids, 
        PUNGraph &net,
        iidinode_map &index_tree,
        eiid_map &index_hash) {
    unordered_set<QR, QR_hash> res;
    for (vector<vid_type>::const_iterator 
            citer = query_vids.begin();
            citer != query_vids.end();
            ++ citer) {
        vid_type u = *citer;
        int u_deg = net->GetNI(u).GetDeg();
        for (int i = 0; i < u_deg; i++) {
            vid_type v = net->GetNI(u).GetNbrNId(i);
            inode_id_type iid = index_hash[edge_composer(u, v)];
            vector<inode_id_type> branch_nodes = branch_search(iid, index_tree); 
            for (size_t j = 0; j < branch_nodes.size(); j ++) {
                inode_id_type jiid = branch_nodes[j];
                QR res_node(jiid, index_tree[jiid].size, index_tree[jiid].k);
                res.insert(res_node);
            }
        }
    }
    return res;
}

#endif
