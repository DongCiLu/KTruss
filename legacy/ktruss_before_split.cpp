#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <queue>
#include <iterator>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <time.h>
#include <boost/functional/hash.hpp>

#include "Snap.h"

using namespace std;

typedef int vid_type;
typedef pair<vid_type, vid_type> eid_type;
typedef vid_type inode_id_type;
struct inode{
    inode_id_type parent;
    size_t size;
    int k;
    
    inode(inode_id_type parent = -1, size_t size = -1, int k = -1): 
        parent(parent), size(size), k(k) { }
};
typedef unordered_map<eid_type, int, boost::hash<eid_type> > eint_map;
typedef unordered_map<eid_type, inode_id_type, boost::hash<eid_type> > 
eiid_map;
typedef unordered_map<inode_id_type, inode> iidinode_map;

/*
 * we have to use hash table as a brute-force scheme 
 * such as A * max + B soon reaches the limit of int.
 * But all we need is a int number for each edge, 
 * Note boost hash or std hash will hash to size_t values,
 * which may lead to collision when cast to int.
 * To avoid handle this by ourself, we simpley use two tables.
 * TODO: Add a singleton class for this
 */
//vid_type max_vid = 0;
unordered_map<eid_type, vid_type, boost::hash<eid_type> > encode_table;
unordered_map<vid_type, eid_type> decode_table;
int vid_cnt = 0;
int max_net_k = 0;

bool support_sort(const pair<eid_type, int> &a, 
        const pair<eid_type, int> &b) {
    return a.second < b.second;
}

inline eid_type edge_composer(const vid_type &u, const vid_type &v) {
    if (u < v) 
        return make_pair(u, v);
    else
        return make_pair(v, u);
}

inline vid_type mst_vid_composer(
        const vid_type &u, const vid_type &v) {
    eid_type e = edge_composer(u, v);
    if (encode_table.find(e) == encode_table.end()) {
        encode_table.insert(make_pair(e, vid_cnt));
        decode_table.insert(make_pair(vid_cnt, e));
        vid_cnt ++;
    }
    return encode_table[e];
}

inline eid_type edge_extractor(const vid_type &x) {
    return decode_table[x];
}

void get_low_high_deg_vertices(const PUNGraph &net, 
        vid_type src, vid_type dst, 
        vid_type &low, vid_type &high) {
    if (net->GetNI(src).GetDeg() > net->GetNI(dst).GetDeg()) {
        low = dst;
        high = src;
    }
    else {
        low = src;
        high = dst;
    }
}

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

int construct_mst(const PUNGraph &net,
        eint_map &edge_trussness,
        eint_map &triangle_trussness,
        PUNGraph &mst) {
    /* 
     * In this new graph, edges become vertex.
     * triangles become edges,
     * vertex weight is edge trussness
     * edge weight is triangle trussness
     */
    unordered_map<vid_type, vid_type> cc;
    unordered_map<vid_type, int> rank;
    set< pair<int, eid_type> > sorted_triangle_trussness;

    /*
    // calc max vid
    for (TUNGraph::TNodeI NI = net->BegNI(); NI < net->EndNI(); NI++) {
        if (NI.GetId() > max_vid)
            max_vid = NI.GetId();
    }
    */

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

void construct_index_tree(const PUNGraph &mst, 
        eint_map &triangle_trussness,
        iidinode_map &index_tree,
        eiid_map &index_hash) {
    vector<int> case_cnt = {0,0,0,0};
    unordered_set<vid_type> unvisited_vertices;
    for (TUNGraph::TNodeI NI = mst->BegNI(); NI < mst->EndNI(); NI++) {
        unvisited_vertices.insert(NI.GetId());
    }

    queue<vid_type> fifo;
    unordered_map<vid_type, vid_type> parents;
    while(!unvisited_vertices.empty()) {
        vid_type next_root = *(unvisited_vertices.begin());
        fifo.push(next_root);
        unvisited_vertices.erase(next_root);
        while(!fifo.empty()) {
            vid_type u = fifo.front();
            fifo.pop();
            int u_deg = mst->GetNI(u).GetDeg();
            int node_k = -1;

            // add children to fifo
            for (int i = 0; i < u_deg; i++) {
                vid_type v = mst->GetNI(u).GetNbrNId(i);
                eid_type e = edge_composer(u,v);
                // the k for each vertex is the highest k it connects
                // we can read it from edge trussness of orig graph?
                if (triangle_trussness[e] > node_k)
                    node_k = triangle_trussness[e];
                if (unvisited_vertices.find(v) == 
                        unvisited_vertices.end())
                    continue;
                fifo.push(v);
                unvisited_vertices.erase(v);
                parents.insert(make_pair(v, u));
            }

            bool processed = false;
            int edge_k = -1;
            vid_type v = -1;
            inode_id_type v_inode_id = -1;
            int v_k = -1;
            inode_id_type lower_inode_id = -1;
            if (parents.find(u) !=  parents.end()) {
                v = parents[u];
                eid_type e = edge_composer(u,v);
                edge_k = triangle_trussness[e];
                v_inode_id = index_hash[edge_extractor(v)];
                v_k = index_tree[v_inode_id].k;
            }
            else {
                // create inode based on u
                inode in(-1, 1, node_k);
                // add inode to the index tree and update hashtable
                index_tree.insert(make_pair(u, in));
                index_hash.insert(make_pair(edge_extractor(u), u));
                processed = true;
            } 

            while (!processed) {
                if (v_k < edge_k) {
                    if (edge_k == node_k) {
                        case_cnt[0] ++;
                        // create inode based on u
                        inode in(v_inode_id, 0, node_k);
                        in.parent = v_inode_id;
                        int lower_size = 1;
                        if (lower_inode_id != -1)
                            lower_size += 
                                index_tree[lower_inode_id].size;
                        in.size = lower_size;
                        // add to the index tree and update hashtable
                        index_tree.insert(make_pair(u, in));
                        index_hash.insert(
                                make_pair(edge_extractor(u), u));
                        // link lower id with this cluster
                        if (lower_inode_id != -1)
                            index_tree[lower_inode_id].parent = u;

                        // update parent size
                        do {
                            index_tree[v_inode_id].size ++; 
                            v_inode_id = index_tree[v_inode_id].parent;
                        } while (v_inode_id != -1);
                        // mark as processed
                        processed = true;
                    }
                    else if (edge_k < node_k) {
                        case_cnt[1] ++;
                        // create inode based on e
                        inode in(v_inode_id, 0, edge_k);
                        int lower_size = 1;
                        if (lower_inode_id != -1)
                            lower_size += 
                                index_tree[lower_inode_id].size;
                        in.size = lower_size;
                        // only add inode as there is no 
                        // vertex related to this inode
                        // temporary solution for edge_cluster
                        int e_inode_id = u * 10000;
                        index_tree.insert(make_pair(e_inode_id, in));
                        // link lower id with this edge cluster
                        if (lower_inode_id != -1)
                            index_tree[lower_inode_id].parent = 
                                e_inode_id;

                        // create inode based on u
                        // reuse the "in" here
                        in.parent = e_inode_id;
                        in.size = 1;
                        in.k = node_k;
                        // add to the index tree and update hashtable
                        index_tree.insert(make_pair(u, in));
                        index_hash.insert(
                                make_pair(edge_extractor(u), u));

                        // update parent size
                        do {
                            index_tree[v_inode_id].size ++; 
                            v_inode_id = index_tree[v_inode_id].parent;
                        } while (v_inode_id != -1);
                        // mark as processed
                        processed = true;
                    }
                    else {
                        cout << "wrong case 1" << endl;
                        processed = true;
                    }
                }
                else if (v_k == edge_k) {
                    if (edge_k == node_k) {
                        case_cnt[2] ++;
                        // update hash table
                        index_hash.insert(make_pair(
                                    edge_extractor(u), v_inode_id));
                        // update parent size
                        do {
                            index_tree[v_inode_id].size ++; 
                            v_inode_id = index_tree[v_inode_id].parent;
                        } while (v_inode_id != -1);
                        // mark as processed
                        processed = true;
                    }
                    else if (edge_k < node_k) { 
                        case_cnt[3] ++;
                        // create inode based on u
                        inode in(v_inode_id, 1, node_k);
                        // add to the index tree and update hashtable
                        index_tree.insert(make_pair(u, in));
                        index_hash.insert(
                                make_pair(edge_extractor(u), u));
                        // update parent size
                        do {
                            index_tree[v_inode_id].size ++; 
                            v_inode_id = index_tree[v_inode_id].parent;
                        } while (v_inode_id != -1);
                        // mark as processed
                        processed = true;
                    }
                    else{
                        cout << "wrong case 2" << endl;
                        processed = true;
                    }
                }
                else {
                    // moving to parent node on index tree
                    lower_inode_id = v_inode_id;
                    v_inode_id = index_tree[v_inode_id].parent;
                    if (v_inode_id == -1)
                        v_k = -1;
                    else
                        v_k = index_tree[v_inode_id].k;
                } //TODO: what if there is no parent?
            }
        }
    }
    for (int i = 0; i < 4; i ++) {
        cout << "case count " << i << ": " << case_cnt[i] << endl;
    }
}

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

vector<vid_type> load_testcases(string filename, int n_test){
    vector<vid_type> testcases;
    set<vid_type> visited_vertices;
    ifstream in(filename.c_str());
    int cnt = 0;
    while(cnt < n_test) {
        int i, j;
        in >> i >> j;
        if (visited_vertices.find(i) == visited_vertices.end()) {
            testcases.push_back(i);
            visited_vertices.insert(i);
            cnt ++;
        }
    }

    return testcases;
}

pair<int, int> calculate_trussness_bounds(const PUNGraph &net,
        eint_map &edge_trussness,
        eid_type e) {
    vid_type u;
    vid_type v;
    get_low_high_deg_vertices(net, e.first, e.second, u, v);
    vector<int> num_k_triangles(max_net_k + 1, 0);
    for (int i = 0; i < net->GetNI(u).GetDeg(); i++) {
        vid_type w = net->GetNI(u).GetNbrNId(i);
        if (net->IsEdge(w, v)) {
            int min_k = min(edge_trussness[edge_composer(u, w)], 
                    edge_trussness[edge_composer(v, w)]);
            num_k_triangles[min_k] += 1;
        }
    }
    int k1 = 0;
    int k2 = 0;
    for (int k = max_net_k; k > 0; k --) {
        if (num_k_triangles[k] >= k - 2 && k1 == 0)
            k1 = k;
        if (num_k_triangles[k - 1] >= k - 2 && k2 == 0)
            k2 = k;
        if (k1 != 0 && k2 != 0)
            break;
    }
    return make_pair(k1, k2);
}

size_t update_edge_trussness(const PUNGraph &net,
        eint_map &edge_trussness,
        unordered_set< eid_type, boost::hash<eid_type> > &updated_edge,
        eid_type new_e) {
    size_t updated_edge_cnt = 0;
    if (net->IsEdge(new_e.first, new_e.second)) {
        cout << "Edge already exist" << endl;
        return updated_edge_cnt;
    }
    net->AddEdge(new_e.first, new_e.second);
    pair<int, int> bounds = calculate_trussness_bounds(net, edge_trussness, new_e);
    edge_trussness[new_e] = bounds.first;
    int k_max = bounds.second - 1;
    unordered_map<eid_type, int, boost::hash<eid_type> > k_level_triangles;

    vector< set<eid_type> > potential_edges;
    for (int k = 0; k < k_max + 1; k ++) {
        potential_edges.push_back(set<eid_type>());
    }
    // first add adjacent edges
    vid_type u;
    vid_type v;
    get_low_high_deg_vertices(net, e.first, e.second, u, v);
    for (int i = 0; i < net->GetNI(u).GetDeg(); i++) {
        vid_type w = net->GetNI(u).GetNbrNId(i);
        if (net->IsEdge(w, v)) {
            int min_k = min(edge_trussness[edge_composer(u, w)], 
                    edge_trussness[edge_composer(v, w)]);
            if (min_k <= k_max) {
                if (edge_trussness[edge_composer(u, w)] == min_k)
                    potential_edges[min_k].insert(edge_composer(u, w));
                if (edge_trussness[edge_composer(v, w)] == min_k)
                    potential_edges[min_k].insert(edge_composer(v, w));
            }
        }
    }
    // expand the seeding 
    for (int k = k_max; k >= 2; k --) {
        queue<eid_type> Q;
        for (set<eid_type>::iterator iter = potential_edges[k].begin();
                iter != potential_edges[k].end();
                ++ iter) {
            Q.push(*iter);
        }
        while (!Q.empty()) {
            eid_type e = Q.front();
            Q.pop();
            k_level_triangles[e] = 0;
            vid_type u = 0;
            vid_type v = 0;
            get_low_high_deg_vertices(net, e.first, e.second, u, v);
            for (int i = 0; i < net->GetNI(u).GetDeg(); i++) {
                vid_type w = net->GetNI(u).GetNbrNId(i);
                if (net->IsEdge(w, v)) {
                    if (edge_trussness[edge_composer(u, w)] < k or
                            edge_trussness[edge_composer(v, w)] < k) 
                        continue;
                    k_level_triangles[e] += 1;
                    if (edge_trussness[edge_composer(u, w)] == k &&
                            potential_edges[k].find(edge_composer(u, w)) == 
                                potential_edges[k].end()) {
                        potential_edges[k].insert(edge_composer(u, w));
                        Q.push(edge_composer(u, w));
                    }
                    if (edge_trussness[edge_composer(v, w)] == k &&
                            potential_edges[k].find(edge_composer(v, w)) == 
                                potential_edges[k].end()) {
                        potential_edges[k].insert(edge_composer(v, w));
                        Q.push(edge_composer(v, w));
                    }
                }
            }
        }
        // remove unqualified edges
        for (set<eid_type>::iterator iter = potential_edges[k].begin();
                iter != potential_edges[k].end();
                ++ iter) {
            if (k_level_triangles[*iter] <= k - 2) 
                Q.push(*iter);
        }
        while (!Q.empty()) {
            eid_type e = Q.front();
            Q.pop();
            potential_edges[k].erase(e);
            get_low_high_deg_vertices(net, e.first, e.first, u, v);
            for (int i = 0; i < net->GetNI(u).GetDeg(); i++) {
                vid_type w = net->GetNI(u).GetNbrNId(i);
                if (net->IsEdge(w, v)) {
                    if (edge_trussness[edge_composer(u, w)] < k or
                            edge_trussness[edge_composer(v, w)] < k) 
                        continue;
                    if (edge_trussness[edge_composer(u, w)] == k &&
                            potential_edges[k].find(edge_composer(u, w)) == 
                                potential_edges[k].end()) 
                        continue;
                    if (edge_trussness[edge_composer(v, w)] == k &&
                            potential_edges[k].find(edge_composer(v, w)) == 
                                potential_edges[k].end())
                        continue;
                    if (potential_edges[k].find(edge_composer(u, w)) != 
                                potential_edges[k].end()) 
                        k_level_triangles[edge_composer(u, w)] -= 1;
                    if (potential_edges[k].find(edge_composer(v, w)) != 
                                potential_edges[k].end())
                        k_level_triangles[edge_composer(v, w)] -= 1;
                }
            }
        }
        // the actual update
        for (set<eid_type>::iterator iter = potential_edges[k].begin();
                iter != potential_edges[k].end();
                ++ iter) {
            edge_trussness[*iter] = k + 1;
            updated_edge.insert(*iter);
            updated_edge_cnt += 1;
        }
    }
    return updated_edge_cnt;
}

size_t update_edge_mst(const PUNGraph &net,
        PUNGraph &mst,
        eint_map &edge_trussness,
        eint_map &triangle_trussness,
        unordered_set< eid_type, boost::hash<eid_type> > &updated_edge,
        eid_type new_e) {

    vid_type u = -1, v = -1;
    get_low_high_deg_vertices(net, new_e.first, new_e.second, u, v);
    int u_deg = net->GetNI(u).GetDeg();
    // add edge as a new vertex to mst
    vid_type v1 = mst_vid_composer(u,v);
    mst->AddNode(v1);

    for (int i = 0; i < u_deg; i++) {
        vid_type w = net->GetNI(u).GetNbrNId(i);
        int uvw_trussness = std::min(
                edge_trussness[edge_composer(u,v)],
                edge_trussness[edge_composer(u,w)]);
        uvw_trussness = std::min(uvw_trussness, 
                edge_trussness[edge_composer(v,w)]);
        vid_type v2 = mst_vid_composer(u,w);
        vid_type v3 = mst_vid_composer(w,v);
        eid_type e1 = edge_composer(v1, v2);
        eid_type e2 = edge_composer(v2, v3);
        eid_type e3 = edge_composer(v1, v3);
        triangle_trussness[e1] = uvw_trussness;
        triangle_trussness[e2] = uvw_trussness;
        triangle_trussness[e3] = uvw_trussness;
    }
}

int main(int argc, char** argv){
    eint_map edge_support;
    eint_map edge_trussness;

    PUNGraph mst = TUNGraph::New();
    eint_map triangle_trussness;

    iidinode_map index_tree;
    eiid_map index_hash;

    vector<vid_type> testcases;
    clock_t start_time = clock();

    if (argc < 3) {
        cout << 
            "USAGE: ./ktruss [graphfile] [testcasesfile]\n" << endl;
        return -1;
    }

    string testcase_file = argv[2];
    int number_of_tests = 10;
    int query_k = -1;
    if (argc > 3) {
        stringstream(argv[3]) >> query_k;
    }

    cout << "1. Loading graph and test cases" << endl;
    PUNGraph net = TSnap::LoadEdgeList<PUNGraph>(argv[1], 0, 1);
    testcases = load_testcases(testcase_file, number_of_tests);
    cout << "Graph size: " << net->GetNodes() << 
        " " << net->GetEdges() << endl;
    cout << "elapsed time: " << 
        double(clock() - start_time) / CLOCKS_PER_SEC << endl;
    start_time = clock();

    cout << "2. Compute support" << endl;
    set< pair<int, eid_type> > sorted_edge_support; // value-key
    compute_support(net, edge_support, sorted_edge_support);
    cout << "elapsed time: " << 
        double(clock() - start_time) / CLOCKS_PER_SEC << endl;
    start_time = clock();
          
    cout << "3. Truss Decomposition (Index Construction)" << endl;
    max_net_k = 
        compute_trussness(net, edge_support, sorted_edge_support, edge_trussness);
    cout << "elapsed time: " << 
        double(clock() - start_time) / CLOCKS_PER_SEC << endl;
    start_time = clock();

    cout << "4. Build MST from generated graph" << endl;
    int num_cc = 
        construct_mst(net, edge_trussness, triangle_trussness, mst);
    if (mst->GetNodes() != net->GetEdges() || mst->GetEdges() != mst->GetNodes() - num_cc) { 
        cout << "Incorrect mst constructed." << endl;
        cout << mst->GetNodes() << "-" << net->GetEdges() << " " << mst->GetEdges() << "-" << mst->GetNodes() - num_cc << endl;
    }
    else {
        cout << "MST has " << mst->GetNodes() << " nodes, "
            << mst->GetEdges() << " edges and "
            << num_cc << " connected components." << endl;
    }
    cout << "elapsed time: " << 
        double(clock() - start_time) / CLOCKS_PER_SEC << endl;
    start_time = clock();

    cout << "5. Construct heirachical index" << endl;
    construct_index_tree(mst, triangle_trussness, 
            index_tree, index_hash);
    cout << index_tree.size() << " " << index_hash.size() << endl;
    cout << "elapsed time: " << 
        double(clock() - start_time) / CLOCKS_PER_SEC << endl;
    start_time = clock();

    cout << "6. K-Truss Query Processing" << endl;
    if (query_k > 0) {
        cout << query_k << "-Truss query" << endl;
        int cnt = 0;
        vector< set<eid_type> > truss_communities;
        QR res;
        for (int i = 0; i < number_of_tests; i++) {
            if (cnt ++ % 10 == 9)
                cout << "." << flush;
                truss_communities = truss_raw_query(
                        query_k, testcases[i], net, edge_trussness);
                vector<vid_type> vids;
                vids.push_back(testcases[i]);
                //res = truss_k_query(
                        //query_k, vids, net, index_tree, index_hash);
        }
        cout << endl;
        cout << "elapsed time (average): " << 
            double(clock() - start_time) / CLOCKS_PER_SEC / 
            number_of_tests << endl;
        start_time = clock();
        cout << "noc: " << truss_communities.size() << endl;
        for (size_t i = 0; i < truss_communities.size(); i++)
            cout << truss_communities[i].size() << " ";
        cout << endl;
        cout << "res: " << res.size << endl;
    }
    else if (query_k == 0){
        cout << "Any-K-truss query" << endl;
    }
    else {
        cout << "Max-K-Truss query" << endl;
    }

    return 0;
}

