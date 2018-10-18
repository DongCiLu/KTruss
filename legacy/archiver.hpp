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
    /*
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
    */

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
        itout << "# " << iter->first << " " 
            << iter->second.parent << " " 
            << iter->second.size << " " 
            << iter->second.k << endl; 
        itout << "* ";
        for (auto item: iter->second.children) {
            itout << item << " ";
        }
        itout << endl;
        for (auto aliter = iter->second.adj_list.begin();
                aliter != iter->second.adj_list.end();
                ++ aliter) {
            itout << aliter->first << " ";
            for (auto nbr: aliter->second) {
                itout << nbr << " ";
            }
            itout << endl;
        }
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
    inode_id_type cur = -1;
    while(itin.good()) {
        string line;
        getline(itin, line);
        if (line.empty())
            continue;
        stringstream ss(line);
        if (line[0] == '#') {
            string start_signal = "";
            inode_id_type u = 0, v = 0;
            size_t size = 0;
            int k = 0;
            ss >> start_signal >> u >> v >> size >> k;
            index_tree.insert(make_pair(u, inode(v, size, k)));
            cur = u;
        }
        else if (line[0] == '*') {
            inode_id_type c = 0;
            string start_signal = "";
            ss >> start_signal;
            while (ss >> c) {
                index_tree[cur].children.push_back(c);
            }
        }
        else {
            vid_type u = 0, v = 0;
            ss >> u;
            index_tree[cur].adj_list[u] = vector<vid_type>();
            while (ss >> v) {
                index_tree[cur].adj_list[u].push_back(v);
            }
        }
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

void save_tcp_index(tcp_index_table_type &tcp_index, 
        string graph_filename,
        string checkpoint_dir) {
    string tcp_index_filename = generate_filename(
            checkpoint_dir, graph_filename, "_tcp_index");
    ofstream tout(tcp_index_filename.c_str());
    for (tcp_index_table_type::iterator 
            iter = tcp_index.begin();
            iter != tcp_index.end();
            ++ iter) {
        tout << iter->first << " ";
        // omit vertices with zero degree
        for (eint_map::iterator 
                eiter = iter->second.ego_triangle_trussness.begin();
                eiter != iter->second.ego_triangle_trussness.end();
                ++ eiter) {
            pair<vid_type, vid_type> vpair = vertex_extractor(eiter->first);
            tout << vpair.first << " " 
                << vpair.second << " " 
                << eiter->second << " ";
        }
        tout << endl;
    }
    tout.close();
}

void load_tcp_index(tcp_index_table_type &tcp_index, 
        string graph_filename,
        string checkpoint_dir) {
    string tcp_index_filename = generate_filename(
            checkpoint_dir, graph_filename, "_tcp_index");
    ifstream tin(tcp_index_filename.c_str());
    while(tin.good()) {
        string line;
        getline(tin, line);
        if (line.empty())
            continue;
        stringstream ss(line);
        vid_type u = -1;
        ss >> u;
        tcp_index_type tcpi;
        tcpi.ego_graph = TUNGraph::New();
        while (!ss.eof()){
            vid_type v1 = -1, v2 = -1;
            int k = -1;
            ss >> v1 >> v2 >> k;
            if (v1 != -1 && v2 != -1 && k != -1) {
                tcpi.ego_graph->AddEdge2(v1, v2); // add nodes automatically
                tcpi.ego_triangle_trussness[edge_composer(v1, v2)] = k;
            }
        }
        tcpi.ego_graph->Defrag();
        tcp_index[u] = tcpi;
    }
    tin.close();
}

void compress(PUNGraph mst,
              eint_map &triangle_trussness,
              unordered_map<eid_type, vid_type> &encode_table,
              unordered_map<vid_type, eid_type> &decode_table,
              iidinode_map &index_tree,
              eiid_map &index_hash,
              string graph_filename,
              string checkpoint_dir) {
    if (triangle_trussness.size() != mst->GetEdges()) {
        cout << "ERROR, not equal edge size: " << 
            triangle_trussness.size() << " " << mst->GetEdges() << endl;
        return;
    }
    if (index_hash.size() != mst->GetNodes()) {
        cout << "ERROR, not equal vertex size: " <<
            index_hash.size() << " " << mst->GetNodes() << endl;
        return;
    }

    string compressed_index1_filename = generate_filename(
            checkpoint_dir, graph_filename, "_compressed1");
    string compressed_index2_filename = generate_filename(
            checkpoint_dir, graph_filename, "_compressed2");
    ofstream cpout1(compressed_index1_filename.c_str());
    ofstream cpout2(compressed_index2_filename.c_str());
    for (auto item: triangle_trussness) {
        pair<vid_type, vid_type> vpair = vertex_extractor(item.first);
        cpout1 << vpair.first << "," << vpair.second << "," 
            << item.second << ",";
        if (index_hash.find(edge_extractor(vpair.first)) != index_hash.end()) {
            if (index_hash[edge_extractor(vpair.first)] != -1) 
                cpout2 << index_hash[edge_extractor(vpair.first)] << ",";
            index_hash.erase(edge_extractor(vpair.first));
        }
        else cpout2 << ",";
        if (index_hash.find(edge_extractor(vpair.second)) != index_hash.end()) {
            if (index_hash[edge_extractor(vpair.second)] != -1)
                cpout2 << index_hash[edge_extractor(vpair.second)];
            index_hash.erase(edge_extractor(vpair.second));
        }
        cpout1 << endl;
        cpout2 << endl;
    }

    if (index_hash.size() != 0) {
        for (auto item: index_hash)
            if (item.second != -1)
                cout << "ERROR, non-trivial entry: " 
                    << item.first << " " << item.second << endl;
    }

    cpout2 << "----" << endl;
    for (auto iter = index_tree.begin();
            iter != index_tree.end();
            ++ iter) {
        cpout2 << iter->first << "," 
            << iter->second.parent << "," 
            << iter->second.size << "," 
            << iter->second.k << endl;
    }

    cpout1.close();
    cpout2.close();
}

#endif
