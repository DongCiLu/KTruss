#include "common.hpp"

#ifndef TREE_INDEX_HPP
#define TREE_INDEX_HPP

void update_inode_size(inode_id_type in_id, iidinode_map &index_tree) {
    while(in_id != -1) {
        index_tree[in_id].size ++; 
        in_id = index_tree[in_id].parent;
    } 
}

void add_new_inode(inode_id_type id,
        inode_id_type parent,
        size_t size,
        int k,
        iidinode_map &index_tree,
        eiid_map &index_hash, 
        bool edge_inode = false) {
    // create inode based on id, here the id is the vertex or edge used to create inode
    inode in(parent, size, k);
    cout << "created a new inode with id " << id 
        << " and parent " << parent
        << " and weight " << k << endl;
    // add inode to the index tree and update hashtable
    index_tree.insert(make_pair(id, in));
    if (!edge_inode) {
        index_hash.insert(make_pair(edge_extractor(id), id));
        update_inode_size(parent, index_tree);
    }
}

// TODO: return some value
void construct_index_tree(const PUNGraph &mst, 
        eint_map &edge_trussness,
        eint_map &triangle_trussness,
        iidinode_map &index_tree,
        eiid_map &index_hash) {
    // we have a virtual root which is the parent of all the roots 
    // TODO: store the total size in it, or not
    vector<int> case_cnt = {0,0,0,0,0};
    // create a container to store unvisited vertices, O(V_mst) = O(E)
    unordered_set<vid_type> unvisited_vertices;
    for (TUNGraph::TNodeI NI = mst->BegNI(); NI < mst->EndNI(); NI++) {
        unvisited_vertices.insert(NI.GetId());
    }
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
            inode_id_type v_inode_id = -1;
            int v_k = -1;
            inode_id_type lower_inode_id = -1;
            // if there is no parent of u, create an inode for u, with inode id equals u
            if (parents.find(u) ==  parents.end()) {
                add_new_inode(u, -1, 1, node_k, index_tree, index_hash);
                /*
                // create inode based on u
                inode in(-1, 1, node_k);
                inode_id_type in_id = u;
                cout << "created a new inode with id " << in_id 
                    << " and parent " << -1 
                    << " and weight " << node_k << endl;
                // add inode to the index tree and update hashtable
                index_tree.insert(make_pair(in_id, in));
                index_hash.insert(make_pair(edge_extractor(u), in_id));
                */
                processed = true;
            } 
            // if there is a parent v, find its status
            else {
                v = parents[u];
                eid_type e = edge_composer(u,v);
                edge_k = triangle_trussness[e];
                v_inode_id = index_hash[edge_extractor(v)];
                v_k = index_tree[v_inode_id].k;
                /*
                cout << node_k << " >= " << edge_k << "; "
                    << v_k << " <= " << edge_k << ": ";
                    */
            }
            // there are 4 cases depends on the trussness of itself, its parent and the edge
            /* 
             * note that in this loop, 
             * the search will first go up the existing tree,
             * to find a proper insert point, 
             * then the vertex u will be inserted to the tree.
             */
            while (!processed) {
                if (v_k > edge_k) {
                    // not a correct insert point, moving to parent node on index tree
                    lower_inode_id = v_inode_id;
                    v_inode_id = index_tree[v_inode_id].parent;
                    if (v_inode_id == -1)
                        v_k = -1;
                    else
                        v_k = index_tree[v_inode_id].k;
                } 
                else if (v_k < edge_k) {
                    if (edge_k == node_k) {
                        //cout << "case 0" << endl;
                        case_cnt[0] ++;
                        // create inode based on u
                        int lower_size = 0;
                        if (lower_inode_id != -1)
                            lower_size += index_tree[lower_inode_id].size;
                        add_new_inode(u, v_inode_id, lower_size + 1, 
                                node_k, index_tree, index_hash);
                        /*
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
                        */
                        // link lower id with this cluster
                        if (lower_inode_id != -1) {
                            cout << "update " << lower_inode_id
                                << "'s parent from " 
                                << index_tree[lower_inode_id].parent
                                << " to " << u << endl;
                            index_tree[lower_inode_id].parent = u;
                        }

                        /*
                        // update parent size
                        do {
                            index_tree[v_inode_id].size ++; 
                            v_inode_id = index_tree[v_inode_id].parent;
                        } while (v_inode_id != -1);
                        */
                        // mark as processed
                        processed = true;
                    }
                    else if (edge_k < node_k) {
                        //cout << "case 1" << endl;
                        case_cnt[1] ++;
                        // create inode based on e
                        // only add inode as there is no 
                        // vertex related to this inode
                        // temporary solution for edge_cluster
                        // TODO: how to set the inode id for large graphs
                        int e_inode_id = u * 10000;
                        bool edge_inode = true;
                        int lower_size = 0;
                        if (lower_inode_id != -1)
                            lower_size = index_tree[lower_inode_id].size;
                        add_new_inode(e_inode_id, v_inode_id, lower_size, 
                                edge_k, index_tree, index_hash, edge_inode);
                        /*
                        inode in(v_inode_id, 0, edge_k);
                        int lower_size = 1;
                        if (lower_inode_id != -1)
                            lower_size += 
                                index_tree[lower_inode_id].size;
                        in.size = lower_size;
                        cout << "created a new edge inode with id " << e_inode_id 
                            << " and parent " << v_inode_id 
                            << " and weight " << edge_k << endl;
                        index_tree.insert(make_pair(e_inode_id, in));
                        */
                        // link lower id with this edge cluster
                        if (lower_inode_id != -1) {
                            cout << "update " << lower_inode_id
                                << "'s parent from " 
                                << index_tree[lower_inode_id].parent
                                << " to " << e_inode_id << endl;
                            index_tree[lower_inode_id].parent = e_inode_id;
                        }

                        // create inode based on u
                        add_new_inode(u, e_inode_id, 1, node_k, index_tree, index_hash);
                        /*
                        // reuse the "in" here
                        in.parent = e_inode_id;
                        in.size = 1;
                        in.k = node_k;
                        // add to the index tree and update hashtable
                        index_tree.insert(make_pair(u, in));
                        index_hash.insert(
                                make_pair(edge_extractor(u), u));
                        */

                        /*
                        // update parent size
                        // TODO: is this logic with v_inode_id correct?
                        do {
                            index_tree[v_inode_id].size ++; 
                            v_inode_id = index_tree[v_inode_id].parent;
                        } while (v_inode_id != -1);
                        */
                        // mark as processed
                        processed = true;
                    }
                    else {
                        cout << "wrong case 1" << endl;
                        processed = true;
                    }
                }
                else { // v_k == edge_k
                    if (edge_k == node_k) {
                        //cout << "case 2" << endl;
                        case_cnt[2] ++;
                        // update hash table
                        index_hash.insert(make_pair(
                                    edge_extractor(u), v_inode_id));
                        update_inode_size(v_inode_id, index_tree);
                        /*
                        // update parent size
                        do {
                            index_tree[v_inode_id].size ++; 
                            v_inode_id = index_tree[v_inode_id].parent;
                        } while (v_inode_id != -1);
                        */
                        // mark as processed
                        processed = true;
                    }
                    else if (edge_k < node_k) { 
                        //cout << "case 3" << endl;
                        case_cnt[3] ++;
                        // create inode based on u
                        add_new_inode(u, v_inode_id, 1, node_k, index_tree, index_hash);
                        /*
                        inode in(v_inode_id, 1, node_k);
                        inode_id_type in_id = u;
                        // add to the index tree and update hashtable
                        cout << "created a new inode with id " << in_id 
                            << " and parent " << v_inode_id 
                            << " and weight " << node_k << endl;
                        index_tree.insert(make_pair(in_id, in));
                        index_hash.insert(
                                make_pair(edge_extractor(u), in_id));
                        */
                        // update parent size
                        /*
                        do {
                            index_tree[v_inode_id].size ++; 
                            v_inode_id = index_tree[v_inode_id].parent;
                        } while (v_inode_id != -1);
                        */
                        // mark as processed
                        processed = true;
                    }
                    else{
                        cout << "wrong case 2" << endl;
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
