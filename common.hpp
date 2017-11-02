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

#endif
