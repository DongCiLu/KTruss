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

#include <cassert>

#include "Snap.h"
#include "simple_graphlib/fileIO.hpp"
#include "simple_graphlib/storage.hpp"
#include "simple_graphlib/graph.hpp"

#ifndef COMMON_HPP
#define COMMON_HPP

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

    bool operator==(const inode& rhs) const {
        return this->parent == rhs.parent && 
            this->size == rhs.size &&
            this->k == rhs.k;
    }
};
typedef unordered_map<eid_type, int, boost::hash<eid_type> > eint_map;
typedef set< pair<int, eid_type> > slow_sorted_type;
typedef vector< list<eid_type> > counting_sorted_type;
typedef unordered_map<eid_type, inode_id_type, boost::hash<eid_type> > eiid_map;
typedef unordered_map<inode_id_type, inode> iidinode_map;

int max_net_k = 0;

void load_graph(graph<vid_type> *net, string graph_filename) {
    net->init();
    file_IO<vid_type> f;
    f.line_parser(graph_filename);
    f.store(net->out, net->in, net->elist);
}

inline eid_type edge_composer(const vid_type &u, const vid_type &v) {
    if (u < v) 
        return make_pair(u, v);
    else
        return make_pair(v, u);
}

void get_low_high_deg_vertices(graph<vid_type> *net, 
        vid_type src, vid_type dst, 
        vid_type &low, vid_type &high) {
    if (net->get_deg(src) > net->get_deg(dst)) {
        low = dst;
        high = src;
    }
    else {
        low = src;
        high = dst;
    }
}

/*
 * We have to use hash table as a brute-force scheme 
 * because A * max + B soon reaches the limit of int.
 * But all we need is a int number for each edge, 
 * note boost hash or std hash will hash to size_t values,
 * which may lead to collision when cast to int.
 * To avoid handle this by ourself, we simpley use two tables.
 */
unordered_map<eid_type, vid_type, boost::hash<eid_type> > encode_table;
unordered_map<vid_type, eid_type> decode_table;
int vid_cnt = 0;

inline vid_type mst_vid_composer(vid_type u, vid_type v) {
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

// virtual node id starts at -2 as 0 and -1 is reserved
inode_id_type reserve_interval = -2; 

// definitions for tcp index
struct TCPIndex {
    PUNGraph ego_graph;
    eint_map ego_triangle_trussness;
};

typedef TCPIndex tcp_index_type;
typedef unordered_map<vid_type, tcp_index_type> tcp_index_table_type;

// definitions for queries
struct QR {
    inode_id_type iid;
    size_t size;
    int k;

    QR(inode_id_type iid = -1, int size = -1, int k = -1): 
        iid(iid), size(size), k(k) { }

    bool operator==(const QR& rhs) const {
        return this->iid == rhs.iid;
    }

    void set(inode_id_type iid, int size, int k) {
        this->iid = iid;
        this->size = size;
        this->k = k;
    }
};

struct qr_hash {
    size_t operator()(const QR& _qr) const {
        return std::hash<inode_id_type>()(_qr.iid);
    }
};

typedef unordered_set<eid_type, boost::hash<eid_type> > community_type;
typedef QR qr_type;
typedef unordered_set<qr_type, qr_hash> qr_set_type;
typedef vector<community_type> exact_qr_set_type;

#endif
