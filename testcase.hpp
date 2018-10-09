#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <algorithm>
#include <ctime>
#include <cstdlib>

#include "common.hpp"

#ifndef TESTCASE_HPP
#define TESTCASE_HPP

#define DEGREE_CUTOFF 20

string get_testcase_filename(string graph_filename, 
                             string testcase_dir,
                             int num_query_vertices) {
    string testcase_filename = testcase_dir;
    size_t pos = graph_filename.rfind("/");
    testcase_filename += graph_filename.substr(pos);
    testcase_filename += "_testcase";
    ostringstream s;
    s << testcase_filename << num_query_vertices;
    return s.str();
}

void save_testcases(PUNGraph net,
                    string graph_filename, 
                    string testcase_dir,
                    size_t num_query_vertices) {
    struct stat st;
    if(stat(testcase_dir.c_str(), &st) == -1)
        mkdir(testcase_dir.c_str(), 
                S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    
    vector< pair<int, vid_type> > degree_vertex_set;
    for (TUNGraph::TNodeI NI = net->BegNI(); NI < net->EndNI(); NI++) {
        int degree = NI.GetDeg();
        if (degree < DEGREE_CUTOFF) continue; 
        degree_vertex_set.push_back(make_pair(degree, NI.GetId()));
    }
    sort(degree_vertex_set.begin(), degree_vertex_set.end());

    string testcase_filename = get_testcase_filename(
            graph_filename, testcase_dir, num_query_vertices);
    ofstream out(testcase_filename.c_str());
    srand(time(NULL));
    size_t n_bucket = 10;
    size_t query_per_bucket = 100;
    size_t bucket_size = degree_vertex_set.size() / n_bucket;
    int max_degree = 0;
    for (size_t i = 0; i < n_bucket; i ++) {
        for (size_t j = 0; j < query_per_bucket; j ++) {
            unordered_set<vid_type> query_vertices;
            while(query_vertices.size() < num_query_vertices) {
                size_t index = rand() % bucket_size + bucket_size * i;
                if (degree_vertex_set[index].first > max_degree) 
                    max_degree = degree_vertex_set[index].first;
                query_vertices.insert(degree_vertex_set[index].second);
            }
            for (vid_type v: query_vertices) {
                out << v << " ";
            }
            out << endl;
        }
        cout << "Degree range in this bracket: "
             << degree_vertex_set[bucket_size * i].first
             << " - "
             << degree_vertex_set[bucket_size * i + bucket_size - 1].first
             << " with " << bucket_size << " vertices." 
             << endl;
    }
    cout << "max degree in test case: " << max_degree << endl;
    out.close();
}

size_t load_testcases(string filename, 
        size_t n_queries, 
        vector<vector<vid_type>> &testcases){
    ifstream in(filename.c_str());
    while(in.good() && testcases.size() < n_queries) {
        string test_line;
        if(in.eof()) {
            cout << "Not enough test cases, "
                << "reach end of testcase file"<< endl;
            break;
        }
        getline(in, test_line);
        if (test_line.empty())
            continue;
        stringstream ss(test_line);
        vector<vid_type> query_vertices;
        int test = -1;
        while(ss >> test)
            query_vertices.push_back(test);
        testcases.push_back(query_vertices);
    }
    in.close();

    return testcases.size();
}

#endif
