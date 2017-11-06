#include "common.hpp"

#ifndef TREE_INDEX_HPP
#define TREE_INDEX_HPP

/*
 * We have to use hash table as a brute-force scheme 
 * because A * max + B soon reaches the limit of int.
 * But all we need is a int number for each edge, 
 * note boost hash or std hash will hash to size_t values,
 * which may lead to collision when cast to int.
 * To avoid handle this by ourself, we simpley use two tables.
 */
// TODO: return some value
void construct_index_tree(const PUNGraph &mst, 
        eint_map &triangle_trussness,
        iidinode_map &index_tree,
        eiid_map &index_hash) {
    // we have a virtual root which is the parent of all the roots 
    // TODO: store the total size in it, or not
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
                cout << "created a new inode with id " << u 
                    << " and parent " << -1 
                    << " and weight " << node_k << endl;
                cout << "1index tree size: " << index_tree.size() << endl;
                        cout << "v_inode_id: " << v_inode_id << endl;
                processed = true;
            } 

            while (!processed) {
                if (v_k < edge_k) {
                    if (edge_k == node_k) {
                        case_cnt[0] ++;
                        // create inode based on u
                        inode in(v_inode_id, 0, node_k);
                        cout << "created a new inode with id " << u 
                            << " and parent " << v_inode_id
                                << " and weight " << node_k << endl;
                        in.parent = v_inode_id;
                        int lower_size = 1;
                        if (lower_inode_id != -1)
                            lower_size += 
                                index_tree[lower_inode_id].size;
                        in.size = lower_size;
                        // add to the index tree and update hashtable
                        index_tree.insert(make_pair(u, in));
                        cout << "2index tree size: " << index_tree.size() << endl;
                        cout << "v_inode_id: " << v_inode_id << endl;
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
                        cout << "created a new inode with id " << e_inode_id 
                            << " and parent " << v_inode_id
                            << " and weight " << edge_k << endl;
                        cout << "3index tree size: " << index_tree.size() << endl;
                        cout << "v_inode_id: " << v_inode_id << endl;
                        // link lower id with this edge cluster
                        if (lower_inode_id != -1)
                            index_tree[lower_inode_id].parent = 
                                e_inode_id;

                        // create inode based on u
                        // reuse the "in" here
                        cout << "created a new inode with id " << u 
                            << " and parent " << e_inode_id
                            << " and weight " << node_k << endl;
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
                        index_tree.insert(make_pair(u, in));
                        cout << "created a new inode with id " << u 
                            << " and parent " << v_inode_id
                            << " and weight " << node_k << endl;
                        cout << "4index tree size: " << index_tree.size() << endl;
                        cout << "v_inode_id: " << v_inode_id << endl;
                        // add to the index tree and update hashtable
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

#endif
