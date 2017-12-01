#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <algorithm>
#include <ctime>
#include <cstdlib>

#include "common.hpp"

#ifndef TESTCASE_HPP
#define TESTCASE_HPP

void save_testcases(PUNGraph net,
        string graph_filename, 
        string testcase_dir,
        string mode) {
    struct stat st;
    if(stat(testcase_dir.c_str(), &st) == -1)
        mkdir(testcase_dir.c_str(), 
                S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    
    string testcase_filename = testcase_dir;
    size_t pos = graph_filename.rfind("/");
    testcase_filename += graph_filename.substr(pos);
    testcase_filename += "_testcase";

    vector< pair<int, vid_type> > degree_vertex_set;
    for (TUNGraph::TNodeI NI = net->BegNI(); NI < net->EndNI(); NI++) {
        int degree = NI.GetDeg();
        degree_vertex_set.push_back(make_pair(degree, NI.GetId()));
    }
    sort(degree_vertex_set.begin(), degree_vertex_set.end());

    if (mode == "single_q") { 
        string testcase_filename_single_q = 
            testcase_filename + "_single_q";
        ofstream out(testcase_filename_single_q.c_str());
        srand(time(NULL));
        size_t n_bucket = 10;
        size_t query_per_bucket = 100;
        size_t bucket_size = degree_vertex_set.size() / n_bucket;
        int max_degree = 0;
        for (size_t i = 0; i < n_bucket; i ++) {
            for (size_t j = 0; j < query_per_bucket; j ++) {
                size_t index = rand() % bucket_size + bucket_size * i;
                if (degree_vertex_set[index].first > max_degree) 
                    max_degree = degree_vertex_set[index].first;
                /*
                while (degree_vertex_set[index].first > 1000) 
                    index = rand() % bucket_size + bucket_size * i;
                    */
                out << degree_vertex_set[index].second << endl;
            }
        }
        cout << "max degree in test case: " << max_degree << endl;
        out.close();
    }
    /*
    else if (mode == "multi_q") { 
        string testcase_filename_multi_q = 
            testcase_filename + "_multi_q";
        ofstream out(testcase_filename_multi_q.c_str());
        srand(time(NULL));
    }
    */
}

size_t load_testcases(string filename, 
        size_t n_queries, 
        vector<vid_type> &testcases,
        string mode = "single_q"){
    if (mode != "single_q" && mode != "multi_q") {
        cerr << "Not supported query type!" << endl;
        return -1;
    }
    ifstream in(filename.c_str());
    while(testcases.size() < n_queries) {
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
        if (mode == "single_q") {
            int test = -1;
            ss >> test;
            testcases.push_back(test);
        }
        else {
            // TODO: implement multi q
            cerr << "Not implemented yet" << endl;
            return -1;
        }
    }
    in.close();

    return testcases.size();
}

#endif
