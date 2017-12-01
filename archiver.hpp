#include <fstream>
#include <sstream>
#include <sys/stat.h>

#include "common.hpp"

#ifndef ARCHIVER_HPP
#define ARCHIVER_HPP

void create_checkpoint_dir(string checkpoint_dir) {
    struct stat st;
    if(stat(checkpoint_dir.c_str(), &st) == -1)
        mkdir(checkpoint_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

string generate_filename(string checkpoint_dir, 
        string graph_filename, string post_fix) {
    string ret = checkpoint_dir;
    size_t pos = graph_filename.rfind("/");
    ret += graph_filename.substr(pos);
    ret += post_fix;
    return ret;
}

void save_edge_trussness(eint_map &edge_trussness, 
        counting_sorted_type &sorted_edge_trussness,
        string graph_filename,
        string checkpoint_dir) {
    string edge_trussness_filename = generate_filename(
            checkpoint_dir, graph_filename, "_trussness");
    ofstream out(edge_trussness_filename.c_str());
    for (eint_map::const_iterator iter = edge_trussness.begin();
            iter != edge_trussness.end();
            ++ iter) {
        out << iter->first << " " 
            << iter->second << endl;
    }
    out.close();
}

void load_edge_trussness(eint_map &edge_trussness, 
        counting_sorted_type &sorted_edge_trussness,
        string graph_filename,
        string checkpoint_dir,
        bool load_sorted = true) {
    string edge_trussness_filename = generate_filename(
            checkpoint_dir, graph_filename, "_trussness");
    ifstream in(edge_trussness_filename.c_str());
    while(in.good()) {
        string line;
        getline(in, line);
        if (line.empty())
            continue;
        stringstream ss(line);
        eid_type e = 0;
        int k = 0;
        ss >> e >> k;
        edge_trussness.insert(make_pair(e, k));
        if (load_sorted) {
            size_t size_k = static_cast<size_t>(k); 
            if (size_k >= sorted_edge_trussness.size())
                sorted_edge_trussness.resize(size_k + 1, list<eid_type>());
            sorted_edge_trussness[k].push_back(e);
        }
    }
    in.close();
}

void save_mst(PUNGraph &mst, 
        eint_map &triangle_trussness,
        unordered_map<eid_type, vid_type> &encode_table,
        unordered_map<vid_type, eid_type> &decode_table,
        string graph_filename,
        string checkpoint_dir) {
    string mst_graph_filename = generate_filename(
            checkpoint_dir, graph_filename, "_mst_graph");
    TFOut gout(mst_graph_filename.c_str());
    mst->Save(gout);

    string mst_weight_filename = generate_filename(
            checkpoint_dir, graph_filename, "_mst_weight");
    ofstream wout(mst_weight_filename.c_str());
    for (eint_map::const_iterator iter = triangle_trussness.begin();
            iter != triangle_trussness.end();
            ++ iter) {
        wout << iter->first<< " " 
            << iter->second << endl;
    }
    wout.close();

    string mst_encode_filename = generate_filename(
            checkpoint_dir, graph_filename, "_mst_hash");
    ofstream eout(mst_encode_filename.c_str());
    for (unordered_map<eid_type, vid_type>::const_iterator 
            iter = encode_table.begin();
            iter != encode_table.end();
            ++ iter) {
        eout << iter->first << " " 
            << iter->second << endl;
    }
    eout.close();
}

void load_mst(PUNGraph &mst, 
        eint_map &triangle_trussness,
        unordered_map<eid_type, vid_type> &encode_table,
        unordered_map<vid_type, eid_type> &decode_table,
        string graph_filename,
        string checkpoint_dir) {
    string mst_graph_filename = generate_filename(
            checkpoint_dir, graph_filename, "_mst_graph");
    TFIn gin(mst_graph_filename.c_str());
    mst = TUNGraph::Load(gin);

    string mst_weight_filename = generate_filename(
            checkpoint_dir, graph_filename, "_mst_weight");
    ifstream win(mst_weight_filename.c_str());
    while(win.good()) {
        string line;
        getline(win, line);
        if (line.empty())
            continue;
        stringstream ss(line);
        eid_type e = 0;
        int k = 0;
        ss >> e >> k;
        triangle_trussness.insert(make_pair(e, k));
    }
    win.close();

    string mst_encode_filename = generate_filename(
            checkpoint_dir, graph_filename, "_mst_hash");
    ifstream ein(mst_encode_filename.c_str());
    while(ein.good()) {
        string line;
        getline(ein, line);
        if (line.empty())
            continue;
        stringstream ss(line);
        eid_type e = 0;
        vid_type w = 0;
        ss >> e >> w;
        encode_table.insert(make_pair(e, w));
        decode_table.insert(make_pair(w, e));
    }
    ein.close();
}

void save_index_tree(iidinode_map &index_tree, 
        eiid_map &index_hash,
        string graph_filename,
        string checkpoint_dir) {
    string it_filename = generate_filename(
            checkpoint_dir, graph_filename, "_index_tree");
    ofstream itout(it_filename.c_str());
    for (iidinode_map::const_iterator iter = index_tree.begin();
            iter != index_tree.end();
            ++ iter) {
        itout << iter->first << " " 
            << iter->second.parent << " " 
            << iter->second.size << " " 
            << iter->second.k << endl;
    }
    itout.close();
    string ih_filename = generate_filename(
            checkpoint_dir, graph_filename, "_index_hash");
    ofstream ihout(ih_filename.c_str());
    for (eiid_map::const_iterator iter = index_hash.begin();
            iter != index_hash.end();
            ++ iter) {
        ihout << iter->first << " " 
            << iter->second << endl;
    }
    ihout.close();
}

void load_index_tree(iidinode_map &index_tree, 
        eiid_map &index_hash,
        string graph_filename,
        string checkpoint_dir) {
    string it_filename = generate_filename(
            checkpoint_dir, graph_filename, "_index_tree");
    ifstream itin(it_filename.c_str());
    while(itin.good()) {
        string line;
        getline(itin, line);
        if (line.empty())
            continue;
        stringstream ss(line);
        inode_id_type u = 0, v = 0;
        size_t size = 0;
        int k = 0;
        ss >> u >> v >> size >> k;
        index_tree.insert(make_pair(u, inode(v, size, k)));
    }
    itin.close();
    string ih_filename = generate_filename(
            checkpoint_dir, graph_filename, "_index_hash");
    ifstream ihin(ih_filename.c_str());
    while(ihin.good()) {
        string line;
        getline(ihin, line);
        if (line.empty())
            continue;
        stringstream ss(line);
        eid_type e = 0;
        vid_type w = 0;
        ss >> e >> w;
        index_hash.insert(make_pair(e, w));
    }
    ihin.close();
}

#endif
