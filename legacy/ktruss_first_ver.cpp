#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <queue>
#include <iterator>
#include <algorithm>
#include <string>
#include <fstream>
#include <cstdlib>
#include <time.h>

#include "Snap.h"

using namespace std;

typedef int vid_type;
typedef pair<int, int> eid_type;

bool support_sort(const pair<eid_type, int> &a, const pair<eid_type, int> &b) {
    return a.second < b.second;
}

eid_type edge_composer(const vid_type &u, const vid_type &v) {
    if (u < v)
        return make_pair(u, v);
    else
        return make_pair(v, u);
}

void get_low_high_deg_vertices(const PUNGraph &net, 
        const vid_type &src, const vid_type &dst, 
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

int compute_support(const PUNGraph &net, const TUNGraph::TEdgeI &EI) {
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

    return support;
}

int compute_trussness(const PUNGraph &net,
        map<eid_type, int> &edge_support, 
        set< pair<int, eid_type> > &sorted_edge_support, 
        map<eid_type, int> &edge_trussness) {
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
                if (edge_support.find(edge_composer(u, w)) != edge_support.end() &&
                        edge_support.find(edge_composer(w, v)) != edge_support.end()) {
                    int uw_support = edge_support[edge_composer(u, w)];
                    sorted_edge_support.erase(
                            make_pair(uw_support, edge_composer(u, w)));
                    edge_support[edge_composer(u, w)] -= 1;
                    uw_support -= 1;
                    sorted_edge_support.insert(
                            make_pair(uw_support, edge_composer(u, w)));
                    int vw_support = edge_support[edge_composer(v, w)];
                    sorted_edge_support.erase(
                            make_pair(vw_support, edge_composer(v, w)));
                    edge_support[edge_composer(v, w)] -= 1;
                    vw_support -= 1;
                    sorted_edge_support.insert(
                            make_pair(vw_support, edge_composer(v, w)));
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

vector< set<eid_type> > truss_query(int query_k, vid_type query_vid, 
        const PUNGraph &net, map<eid_type, int> &edge_trussness) {
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

class TCPIndex {
    public:
        typedef list< set<vid_type> > TCP_vlist_type;
        typedef set< pair<int, eid_type> > TCP_elist_type;
        //typedef vector< pair<eid_type, int> > TCP_Tx_type; 
        typedef map< vid_type, list< pair<vid_type, int> > > TCP_Tx_type; 
        TCPIndex () {}
        TCPIndex (vid_type u, const PUNGraph &net, 
                map<eid_type, int> &edge_trussness) {
            TCP_vlist_type vlist;
            TCP_elist_type elist;
            for (int i = 0; i < net->GetNI(u).GetDeg(); i++) {
                // set up disjoint vertex set for fast connectivity query
                vid_type w = net->GetNI(u).GetNbrNId(i);
                set<vid_type> temp;
                temp.insert(w);
                vlist.push_back(temp);
                // traverse triangle to generate edge list
                vid_type x, y;
                get_low_high_deg_vertices(net, u, w, x, y);
                for (int j = 0; j < net->GetNI(x).GetDeg(); j++) {
                    vid_type z = net->GetNI(x).GetNbrNId(j);
                    if (net->IsEdge(y, z)) {
                        int triangle_trussness = std::min(edge_trussness[edge_composer(x,y)],
                                edge_trussness[edge_composer(x,z)]);
                        triangle_trussness = std::min(edge_trussness[edge_composer(y,z)],
                                triangle_trussness);
                        if (x == u)
                            elist.insert(make_pair(triangle_trussness, edge_composer(y,z)));
                        else
                            elist.insert(make_pair(triangle_trussness, edge_composer(x,z)));
                    }
                }
            }
            // find maximum spanning tree Tx
            for (typename TCP_elist_type::reverse_iterator 
                    riter = elist.rbegin();
                    riter != elist.rend();
                    ++ riter) {
                vid_type x = riter->second.first;
                vid_type y = riter->second.second;
                bool connected = false;
                set<vid_type> setx, sety;
                for (typename TCP_vlist_type::iterator 
                        iter = vlist.begin();
                        iter != vlist.end();) {
                    if (iter->find(x) != iter->end()) {
                        if (iter->find(y) != iter->end()) {
                            connected = true;
                            break;
                        }
                        else {
                            setx = *iter;
                            iter = vlist.erase(iter);
                        }
                    } else if (iter->find(y) != iter->end()) { 
                        sety = *iter; 
                        iter = vlist.erase(iter);
                    }
                    else
                        ++ iter;
                }
                if (connected) {
                    continue;
                }
                else {
                    // set<vid_type> setunion;
                    // std::set_union(setx.begin(), setx.end(),
                    //         sety.begin(), sety.end(), setunion.begin());
                    setx.insert(sety.begin(), sety.end());
                    vlist.push_back(setx);
                    if (Tx.find(x) == Tx.end()) {
                        list< pair<vid_type, int> > temp;
                        Tx[x] = temp;
                    }
                    Tx[x].push_back(make_pair(y, riter->first));
                    if (Tx.find(y) == Tx.end()) {
                        list< pair<vid_type, int> > temp;
                        Tx[y] = temp;
                    }
                    Tx[y].push_back(make_pair(x, riter->first));
                }
            }
        }

        set<vid_type> get_cc(vid_type v, int k) {
            set<vid_type> cc;
            queue<vid_type> fifo;
            fifo.push(v);
            cc.insert(v);
            while(!fifo.empty()) {
                vid_type w = fifo.front();
                fifo.pop();
                for(list< pair<vid_type, int> >::iterator iter = Tx[w].begin();
                        iter != Tx[w].end(); ++iter) {
                    if (iter->second < k || cc.find(iter->first) != cc.end()) 
                        continue;
                    fifo.push(iter->first);
                    cc.insert(iter->first);
                }
            }
            return cc;
        }

        void print() {
            cout << "size: " << Tx.size() << endl;
            for(typename TCP_Tx_type::iterator iter = Tx.begin();
                    iter != Tx.end(); ++iter) {
                cout << iter->first << endl;
                for(list< pair<vid_type, int> >::iterator jter = iter->second.begin();
                        jter != iter->second.end(); ++jter) {
                    cout << "\t" << jter->first <<": " << jter->second << endl;
                }
            }
        }
    private:
        TCP_Tx_type Tx;
};

vector< set<eid_type> > truss_tcp_query(int query_k, vid_type query_vid, 
        const PUNGraph &net, map<eid_type, int> &edge_trussness,
        map<vid_type, TCPIndex> &indexes) {
    vector< set<eid_type> > truss_communities;
    set<eid_type> visited_edges; // this visited edges have directions !!
    for (int i = 0; i < net->GetNI(query_vid).GetDeg(); i++) {
        vid_type u = net->GetNI(query_vid).GetNbrNId(i);
        if (edge_trussness[edge_composer(u, query_vid)] >= query_k &&
                visited_edges.find(edge_composer(u, query_vid)) == visited_edges.end()) {
            set<eid_type> truss_community;
            queue<eid_type> fifo; // edges in this fifo have directions !! 
            fifo.push(make_pair(u, query_vid));
            visited_edges.insert(make_pair(u, query_vid));
            while (!fifo.empty()) {
                eid_type e = fifo.front();
                fifo.pop();
                set<vid_type> cc = indexes[e.first].get_cc(e.second, query_k);
                for (set<vid_type>::iterator iter = cc.begin(); 
                        iter != cc.end(); ++ iter) {
                    visited_edges.insert(make_pair(e.first, *iter));
                    truss_community.insert(edge_composer(e.first, *iter));
                    if (visited_edges.find(make_pair(*iter, e.first)) == visited_edges.end())
                        fifo.push(make_pair(*iter, e.first));
                }
            }
            truss_communities.push_back(truss_community);
        }
    }
    return truss_communities;
}

int k_level_triangle_cnt(PUNGraph &net, eid_type &new_e, int k,
        map<eid_type, int> &edge_trussness) {
    int triangle_cnt = 0;
    vid_type u, v;
    get_low_high_deg_vertices(net, new_e.first, new_e.second, u, v);

    for (int i = 0; i < net->GetNI(u).GetDeg(); i++) {
        vid_type w = net->GetNI(u).GetNbrNId(i);
        if (net->IsEdge(w, v)) {
            if(edge_trussness[edge_composer(u, w)] >= k && 
                    edge_trussness[edge_composer(v, w)] >=k) {
                triangle_cnt ++;
            }
        }
    }
    return triangle_cnt;
}

void update_trussness(PUNGraph &net,
        eid_type &new_e,
        int max_trussness,
        map<eid_type, int> &edge_trussness) {
    net->AddEdge(new_e.first, new_e.second);
    // compute k1 k2 by lemma 2
    int k1 = 0, k2 = 0;
    int triangle_cnt = 0, last_triangle_cnt = 0;
    for (int k = 2; k <= max_trussness; k++) {
        triangle_cnt = k_level_triangle_cnt(net, new_e, k, edge_trussness);
        if (triangle_cnt >= k - 2) {
            if (k > k1) 
                k1 = k;
        }
        if (last_triangle_cnt >= k - 2) {
            if (k > k2) 
                k2 = k;
        }
        last_triangle_cnt = triangle_cnt;
    }
    // prepare adjacent edges in the scope (L)
    edge_trussness[new_e] = k1;
    int kmax = k2 - 1;
    map< int, set<eid_type> > L;
    for (int k = 2; k <= kmax; k++) {
        set<eid_type> temp;
        L.insert(make_pair(k, temp));
    }
    vid_type u, v;
    get_low_high_deg_vertices(net, new_e.first, new_e.second, u, v);
    for (int i = 0; i < net->GetNI(u).GetDeg(); i++) {
        vid_type w = net->GetNI(u).GetNbrNId(i);
        if (net->IsEdge(w, v)) {
            int k = std::min(edge_trussness[edge_composer(u, w)],
                    edge_trussness[edge_composer(v, w)]);
            if (k <= kmax) {
                if (edge_trussness[edge_composer(u, w)] == k)
                    L[k].insert(edge_composer(u, w));
                else
                    L[k].insert(edge_composer(v, w));
            }
        }
    }
    // search for scope and update trussness in the scope
    map<eid_type, int> s;
    for (int k = kmax; k > 2; k--) {
        // edge expansion
        queue<eid_type> fifo;
        for (typename set<eid_type>::iterator iter = L[k].begin();
                iter != L[k].end(); ++iter) {
            fifo.push(*iter);
        }
        while (!fifo.empty()) {
            eid_type e = fifo.front();
            vid_type x,y;
            fifo.pop();
            s[e] = 0;
            get_low_high_deg_vertices(net, e.first, e.second, x, y);
            for (int i = 0; i < net->GetNI(x).GetDeg(); i++) {
                vid_type z = net->GetNI(x).GetNbrNId(i);
                if (net->IsEdge(z, y)) {
                    if (edge_trussness[edge_composer(x, z)] < k ||
                            edge_trussness[edge_composer(y, z)] < k)
                        continue;
                    s[e] ++;
                    if (edge_trussness[edge_composer(x, z)] == k &&
                            L[k].find(edge_composer(x, z)) == L[k].end()) {
                        fifo.push(edge_composer(x, z));
                        L[k].insert(edge_composer(x, z));
                    }
                    if (edge_trussness[edge_composer(y, z)] == k &&
                            L[k].find(edge_composer(y, z)) == L[k].end()) {
                        fifo.push(edge_composer(y, z));
                        L[k].insert(edge_composer(y, z));
                    }
                }
            }
        }
        // edge eviction
        for (typename set<eid_type>::iterator iter = L[k].begin();
                iter != L[k].end();) {
            if (s[*iter] <= k - 2) {
                eid_type e = *iter;
                iter = L[k].erase(iter);
                vid_type x,y;
                get_low_high_deg_vertices(net, e.first, e.second, x, y);
                for (int i = 0; i < net->GetNI(x).GetDeg(); i++) {
                    vid_type z = net->GetNI(x).GetNbrNId(i);
                    if (net->IsEdge(z, y)) {
                        if (edge_trussness[edge_composer(x, z)] < k ||
                                edge_trussness[edge_composer(y, z)] < k)
                            continue;
                        if (edge_trussness[edge_composer(x, z)] == k &&
                            L[k].find(edge_composer(x, z)) == L[k].end()) 
                            continue;
                        if (edge_trussness[edge_composer(y, z)] == k &&
                            L[k].find(edge_composer(y, z)) == L[k].end()) 
                            continue;
                        if(L[k].find(edge_composer(x, z)) != L[k].end()) 
                            s[edge_composer(x, z)] --;
                        if(L[k].find(edge_composer(y, z)) != L[k].end()) 
                            s[edge_composer(y, z)] --;
                    }
                }
            }
            else
                ++ iter;
        }
        // trussness update
        for (typename set<eid_type>::iterator iter = L[k].begin();
                iter != L[k].end(); ++ iter) {
            /*
            cout << "trussness update: " << iter->first << "," << iter->second << ": "
                << edge_trussness[*iter] << " to " << k + 1 << endl;
            */
            edge_trussness[*iter] = k + 1;
        }
    }
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

int main(int argc, char** argv){
    map<eid_type, int> edge_trussness;
    map<eid_type, int> edge_support; // key-value
    set< pair<int, eid_type> > sorted_edge_support; // value-key
    map<vid_type, TCPIndex> indexes;
    vector<vid_type> testcases;
    clock_t start_time = clock();

    if (argc < 3) {
        cout << "USAGE: ./ktruss [graphfile] [testcasesfile]\n" << endl;
        return -1;
    }

    string testcase_file = argv[2];
    int number_of_tests = 100;
    int number_of_updates = 10;
    int query_k = 5;

    cout << "1. Loading graph and test cases" << endl;
    PUNGraph net = TSnap::LoadEdgeList<PUNGraph>(argv[1], 0, 1);
    testcases = load_testcases(testcase_file, number_of_tests);
    cout << "elapsed time: " << double(clock() - start_time) / CLOCKS_PER_SEC << endl;
    start_time = clock();

    cout << "2. Compute support" << endl;
    for (TUNGraph::TEdgeI EI = net->BegEI(); EI < net->EndEI(); EI++) {
        int support = compute_support(net, EI);
        edge_support.insert(make_pair(edge_composer(
                        EI.GetSrcNId(), EI.GetDstNId()), support));
        sorted_edge_support.insert(make_pair(support, edge_composer(
                        EI.GetSrcNId(), EI.GetDstNId())));
    }
    cout << "elapsed time: " << double(clock() - start_time) / CLOCKS_PER_SEC << endl;
    start_time = clock();
          
    cout << "3. Truss Decomposition (Index Construction)" << endl;
    int max_trussness = compute_trussness(
            net, edge_support, sorted_edge_support, edge_trussness);
    cout << "elapsed time: " << double(clock() - start_time) / CLOCKS_PER_SEC << endl;
    start_time = clock();

    cout << "4. K-Truss Query Processing" << endl;
    int cnt = 0;
    for (int i = 0; i < number_of_tests; i++) {
        if (cnt ++ % 10 == 9)
            cout << "." << flush;
        vector< set<eid_type> > truss_communities_normal = 
            truss_query(query_k, testcases[i], net, edge_trussness);
    }
    cout << endl;
    cout << "elapsed time (average): " << 
        double(clock() - start_time) / CLOCKS_PER_SEC / number_of_tests << endl;
    start_time = clock();
    /*
    cout << "noc: " << truss_communities_normal.size() << endl;
    for (size_t i = 0; i < truss_communities_normal.size(); i++)
        cout << truss_communities_normal[i].size() << " ";
    cout << endl;
    */

    cout << "5. TCP Index Construction" << endl;
    for (TUNGraph::TNodeI NI = net->BegNI(); NI < net->EndNI(); NI++) {
        if (cnt ++ % 100 == 99)
            cout << "." << flush;
        TCPIndex index(NI.GetId(), net, edge_trussness);
        indexes.insert(make_pair(NI.GetId(), index));
    }
    cout << endl;
    cout << "elapsed time: " << double(clock() - start_time) / CLOCKS_PER_SEC << endl;
    start_time = clock();

    cout << "6. K-Truss Query Processing using TCP index" << endl;
    for (int i = 0; i < number_of_tests; i++) {
        if (cnt ++ % 10 == 9)
            cout << "." << flush;
        vector< set<eid_type> > truss_communities_tcp = 
            truss_tcp_query(query_k, testcases[i], net, edge_trussness, indexes);
    }
    cout << endl;
    cout << "elapsed time (average): " << 
        double(clock() - start_time) / CLOCKS_PER_SEC / number_of_tests << endl;
    start_time = clock();
    /*
    cout << "noc: " << truss_communities_tcp.size() << endl;
    for (size_t i = 0; i < truss_communities_tcp.size(); i++)
        cout << truss_communities_tcp[i].size() << " ";
    cout << endl;
    */

    cout << "7. Update edge trusness" << endl;
    srand(time(NULL));
    for (int i = 0; i < number_of_updates; i++) {
        vid_type u = 0, v = 0;
        do {
            u = testcases[rand() % testcases.size()];
            v = testcases[rand() % testcases.size()];
        } while(u == v || net->IsEdge(u,v));
        eid_type new_e = make_pair(u, v);
        update_trussness(net, new_e, max_trussness, edge_trussness);
    }
    cout << "elapsed time (average): " << 
        double(clock() - start_time) / CLOCKS_PER_SEC / number_of_tests << endl;
    start_time = clock();
    /*
    for (map<eid_type, int>::iterator iter = edge_trussness.begin();
            iter != edge_trussness.end(); ++ iter) {
        cout << iter->first.first << "," << iter->first.second << 
            ": " << iter->second << endl;
    }
    */

    return 0;
}

