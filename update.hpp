#include "common.hpp"

#ifndef UPDATE_HPP
#define UPDATE_HPP

pair<int, int> calculate_trussness_bounds(graph<vid_type> *net,
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

size_t update_edge_trussness(graph<vid_type> *net,
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
    // change e to new_e for now, not sure if it is correct
    get_low_high_deg_vertices(net, new_e.first, new_e.second, u, v);
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

size_t update_edge_mst(graph<vid_type> *net,
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

    return 0;
}

#endif
