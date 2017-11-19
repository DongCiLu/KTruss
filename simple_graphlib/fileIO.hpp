/*
 * Component: Read raw graph file into memory
 * Author: Zheng Lu
 * Email: zlu12@utk.edu
 * Note: This is a header only class
 */

#ifndef CS594_FILEIO_H
#define CS594_FILEIO_H

//#define FILEIO_H_DEBUG

#include <vector>
#include <utility>
#include <fstream>
#include <sstream>
#include <string>

#ifdef FILEIO_H_DEBUG
#include <iostream>
#endif

#include "storage.hpp"

template <typename id_type>
class file_IO {
    public:
        file_IO() { }

        void set_buffer(std::vector< std::pair<id_type, id_type> > *buffer) {
            this->buffer.swap(*buffer);
        }

        void line_parser(std::string filename) {
            std::ifstream in(filename.c_str());
            std::string line = "";
            //getline(in, line); // discard the first line
            while(getline(in, line)) {
                std::istringstream ss(line);
                id_type src = -1, dst = -1;
                ss >> src;
                ss >> dst;
#ifdef FILEIO_H_DEBUG
                std::cout << "parse edge: " << src << " " << dst << std::endl;
#endif
                buffer.push_back(std::make_pair(src, dst));
            }
            in.close();
        }

        void store(storage<id_type> *out, storage<id_type> *in) {
            out->init(buffer);
            for(size_t i = 0; i < buffer.size(); ++i) 
                swap_pair(buffer[i]);
            in->init(buffer);
        }

    private:
        void swap_pair(std::pair<id_type, id_type> &p) {
            id_type temp = p.first;
            p.first = p.second;
            p.second = temp;
        }
        std::vector< std::pair<id_type, id_type> > buffer;
};

#endif
