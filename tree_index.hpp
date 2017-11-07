#include "common.hpp"

#ifndef TREE_INDEX_HPP
#define TREE_INDEX_HPP

void update_inode_size(inode_id_type iid, iidinode_map &index_tree) {
    while(iid != -1) {
        index_tree[iid].size ++; 
        iid = index_tree[iid].parent;
    } 
}

void update_inode_parent(inode_id_type lower_iid, 
        inode_id_type iid, iidinode_map &index_tree) {
    if (lower_iid != -1) {
        cout << "update " << lower_iid
            << "'s parent from " 
            << index_tree[lower_iid].parent
            << " to " << iid << endl;
        index_tree[lower_iid].parent = iid;
    }
}

void add_new_inode(inode_id_type id,
        inode_id_type parent,
        size_t size,
        int k,
        iidinode_map &index_tree,
        eiid_map &index_hash, 
        bool virtual_inode = false) {
    // create inode based on id, here the id is the vertex or edge used to create inode
    inode in(parent, size, k);
    // add inode to the index tree and update hashtable, negative id means virtual vertices
    index_tree.insert(make_pair(id, in));
    if (!virtual_inode) {
        index_hash.insert(make_pair(edge_extractor(id), id));
        update_inode_size(parent, index_tree);
    }
    cout << "created a new inode with id " << id 
        << " and parent " << parent
        << " and weight " << k << endl;
    cout << "index tree size: " << index_tree.size() << endl;
}

// TODO: return some value
void construct_index_tree(const PUNGraph &mst, 
        eint_map &edge_trussness,
        eint_map &triangle_trussness,
        iidinode_map &index_tree,
        eiid_map &index_hash) {
    // we have a virtual root which is the parent of all the roots 
    // TODO: store the total size in it, or not
    vector<int> case_cnt = {0, 0, 0, 0};
    // create a container to store unvisited vertices, O(V_mst) = O(E)
    unordered_set<vid_type> unvisited_vertices;
    for (TUNGraph::TNodeI NI = mst->BegNI(); NI < mst->EndNI(); NI++) {
        unvisited_vertices.insert(NI.GetId());
    }
    // add the virtual node first
    add_new_inode(-1, -1, 0, -1, index_tree, index_hash, true);
    // perform the BFS to construct tree index, O(E_mst) < O(V_mst) = O(E)
    queue<vid_type> fifo; // fifo for BFS
    unordered_map<vid_type, vid_type> parents; // parent for each vertex
    while(!unvisited_vertices.empty()) {
        // this loop is for the forest, as there might be multiple connected components
        vid_type next_root = *(unvisited_vertices.begin());
        fifo.push(next_root);
        unvisited_vertices.erase(next_root);
        while(!fifo.empty()) {
            // this loop is for the tree, a single connected component
            vid_type u = fifo.front();
            eid_type e_u = edge_extractor(u);
            fifo.pop();
            int u_deg = mst->GetNI(u).GetDeg();
            int node_k = -1;
            // add children to fifo
            for (int i = 0; i < u_deg; i++) {
                vid_type v = mst->GetNI(u).GetNbrNId(i);
                eid_type e = edge_composer(u,v);
                // the k for each vertex is the highest k it connects
                // we can read it from edge trussness of orig graph?
                // I add the test of it below
                if (triangle_trussness[e] > node_k)
                    node_k = triangle_trussness[e];
                if (unvisited_vertices.find(v) == 
                        unvisited_vertices.end())
                    continue;
                fifo.push(v);
                unvisited_vertices.erase(v);
                parents.insert(make_pair(v, u));
            }
            //test it here.
            if (node_k != edge_trussness[e_u]) {
                cerr << "Failed assumption: node weight is "
                    << "not higher than adjacent edge weight" << endl;
                cerr << u << "(" << e_u.first << " " << e_u.second << "): "
                    << node_k << " != " << edge_trussness[e_u] << endl;
            }
            // add itself to the tree index, assume its parent during BFS is v
            bool processed = false;
            int edge_k = -1;
            vid_type v = -1;
            inode_id_type v_iid = -1;
            int v_k = -1;
            inode_id_type lower_iid = -1;
            size_t lower_size = 0;
            // if there is no parent of u, create an inode for u, with u as inode id 
            if (parents.find(u) ==  parents.end()) {
                add_new_inode(u, -1, 1, node_k, index_tree, index_hash);
                processed = true;
            } 
            // if there is a parent v, find its status
            else {
                v = parents[u];
                eid_type e = edge_composer(u,v);
                edge_k = triangle_trussness[e];
                v_iid = index_hash[edge_extractor(v)];
                v_k = index_tree[v_iid].k;
            }
            while (!processed) {
                if (v_k > edge_k) { // first go up the existing tree to find insert point
                    // not a valid insert point, moving to parent node on index tree
                    lower_iid = v_iid;
                    lower_size = index_tree[lower_iid].size;
                    v_iid = index_tree[v_iid].parent;
                    v_k = index_tree[v_iid].k;
                } 
                // insert depends on the trussness of itself, the insert point and the edge
                else if (v_k < edge_k) {
                    if (edge_k == node_k) {
                        case_cnt[0] ++;
                        // create inode based on u
                        add_new_inode(u, v_iid, lower_size + 1, 
                                node_k, index_tree, index_hash);
                        // link lower id with this cluster
                        update_inode_parent(lower_iid, u, index_tree);
                        // mark as processed
                        processed = true;
                    }
                    else if (edge_k < node_k) {
                        case_cnt[1] ++;
                        // create inode based on e as u does not belong to this inode
                        inode_id_type e_iid = -u + reserve_interval;
                        add_new_inode(e_iid, v_iid, lower_size, 
                                edge_k, index_tree, index_hash, true);
                        // link lower id with this edge cluster
                        update_inode_parent(lower_iid, e_iid, index_tree);
                        // create inode based on u
                        add_new_inode(u, e_iid, 1, node_k, index_tree, index_hash);
                        // mark as processed
                        processed = true;
                    }
                    else {
                        cerr << "wrong case 1" << endl;
                        processed = true;
                    }
                }
                else { // v_k == edge_k
                    if (edge_k == node_k) {
                        case_cnt[2] ++;
                        // update hash table
                        index_hash.insert(make_pair(
                                    edge_extractor(u), v_iid));
                        update_inode_size(v_iid, index_tree);
                        // mark as processed
                        processed = true;
                    }
                    else if (edge_k < node_k) { 
                        case_cnt[3] ++;
                        // create inode based on u
                        add_new_inode(u, v_iid, 1, node_k, index_tree, index_hash);
                        // mark as processed
                        processed = true;
                    }
                    else{
                        cerr << "wrong case 2" << endl;
                        processed = true;
                    }
                }
            }
        }
    }
    for (int i = 0; i < 4; i ++) {
        cout << "case count " << i << ": " << case_cnt[i] << endl;
    }
}

#endif
