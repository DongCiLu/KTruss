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
};
typedef unordered_map<eid_type, int, boost::hash<eid_type> > eint_map;
typedef unordered_map<eid_type, inode_id_type, boost::hash<eid_type> > 
eiid_map;
typedef unordered_map<inode_id_type, inode> iidinode_map;
typedef unordered_set<eid_type, boost::hash<eid_type> > community_type;

int max_net_k = 0;

inline eid_type edge_composer(const vid_type &u, const vid_type &v) {
    if (u < v) 
        return make_pair(u, v);
    else
        return make_pair(v, u);
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

#endif
