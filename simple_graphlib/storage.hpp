/*
 * Component: Storage for graph
 * Author: Zheng Lu
 * Email: zlu12@utk.edu
 * Note: This is a header only class
 */

#ifndef CS594_STORAGE_H
#define CS594_STORAGE_H

//#define STORAGE_H_DEBUG

#include <vector>
#include <utility>
#include <iterator>
#include <algorithm>

#ifdef STORAGE_H_DEBUG
#include <iostream>
#endif

/*
 * base class for different kind of storages
 * a pure abstract class
 */

template <typename id_type>
class storage{
    public:
        typedef typename std::vector<id_type>::iterator iterator;

        virtual void init(std::vector< std::pair<id_type, id_type> > &buffer) = 0;
        virtual void clear() = 0;
        virtual iterator begin(id_type k) = 0; 
        virtual iterator end(id_type i) = 0; 
        virtual size_t num_ids() = 0;
        virtual size_t num_vals() = 0;
        virtual size_t num_vals_id(id_type id) = 0; 
        virtual size_t _sizeof() = 0;
#ifdef STORAGE_H_DEBUG
        virtual void print() = 0;
#endif
};

/* 
 * key - value storage
 * 1. Store large scale networks, that's why we use vector instead of map
 * 2. Fast load all values of a key
 * 3. ASSUME we already have 0 to n-1 numbered keys
 */

template <typename id_type>
class csr_storage : public storage<id_type> {
    public:
        typedef typename storage<id_type>::iterator iterator;
        typedef typename std::vector< std::pair<id_type, id_type> >::iterator pair_iterator;
        csr_storage() { }

        virtual void init(std::vector< std::pair<id_type, id_type> > &buffer) {
            // we'd better replace this with counting sort
            std::sort(buffer.begin(), buffer.end());

            // prepare the memory
            indices.reserve(buffer.rbegin()->first + 1);
            values.reserve(buffer.size());

            // store edge buffer to internal containers
            id_type old_key = -1;
            for (pair_iterator iter = buffer.begin(); iter != buffer.end(); ++ iter) {
                if (iter->first != old_key) {
                    for(id_type i = old_key; i < iter->first; ++i) 
                        indices.push_back(iter - buffer.begin());
                    old_key = iter->first;
                }
                values.push_back(iter->second);
            }
        }

        virtual iterator begin(id_type i) {
            return i < indices.size() ? values.begin() + indices[i] : values.end();
        }

        virtual iterator end(id_type i) {
            return (i+1) < indices.size() ? values.begin() + indices[i+1] : values.end();
        }

        virtual size_t num_ids() { return indices.size(); }

        virtual size_t num_vals() { return values.size(); }

        virtual size_t num_vals_id(id_type id) { return end(id) - begin(id); }

        virtual void clear() {
            std::vector<size_t>().swap(indices);
            std::vector<id_type>().swap(values);
        }

        virtual size_t _sizeof() {
            return sizeof(size_t) * indices.capacity() + 
                sizeof(id_type) * values.capacity();
        }

#ifdef STORAGE_H_DEBUG
        virtual void print() {
            std::cout << "Indices: " << std::endl;
            for(size_t i = 0; i < indices.size(); ++i) 
                std::cout << indices[i] << " ";
            std::cout << std::endl;
            std::cout << "Values: " << std::endl;
            for(size_t i = 0; i < values.size(); ++i) 
                std::cout << values[i] << " ";
            std::cout << std::endl;
        }
#endif

    private:
        std::vector<size_t> indices; // store the start position of each key
        std::vector<id_type> values; // store values for all keys
};

/* 
 * Matrix storage
 * 1. Store small scale networks
 * 2. ASSUME we already have 0 to n-1 numbered keys
 */

template <typename id_type>
class matrix_storage : public storage<id_type> {
    public:
        // due to implementation limitations, value and id should have same type 
        typedef id_type value_type; 
        typedef typename storage<id_type>::iterator iterator;
        typedef typename std::vector< std::pair<id_type, id_type> >::iterator pair_iterator;
        matrix_storage() { }

        virtual void init(std::vector< std::pair<id_type, id_type> > &buffer) {
            // Find out how many vertices we have 
            id_type n = 0;
            for (pair_iterator iter = buffer.begin(); iter != buffer.end(); ++ iter) {
                if (iter->first > n)
                    n = iter->first;
                if(iter->second > n)
                    n = iter->second;
            }
            n += 1;
#ifdef STORAGE_H_DEBUG
            std::cout << "stage 1 done with number of v: " << n << std::endl;
#endif

            // prepair the memory
            adj_matrix.reserve(n);
            adj_matrix.resize(n);
            for (size_t i = 0; i < adj_matrix.size(); ++i) {
                adj_matrix[i].reserve(n);
                adj_matrix[i].resize(n, 0);
            }

            // store edge buffer to internal containers
            for (pair_iterator iter = buffer.begin(); iter != buffer.end(); ++ iter) {
                adj_matrix[iter->first][iter->second] = 1;
            }
        }

        virtual iterator begin(id_type i) {
            return adj_matrix[i].begin();
        }

        virtual iterator end(id_type i) {
            return adj_matrix[i].end();
        }

        virtual size_t num_ids() { return adj_matrix.size(); }

        virtual size_t num_vals() { 
            size_t total = 0;
            for (size_t i = 0; i < adj_matrix.size(); ++i) {
                for (size_t j = 0; j < adj_matrix[i].size(); ++j) {
                    if (adj_matrix[i][j] == 1)
                        total ++;
                }
            }

            return total; 
        }

        virtual size_t num_vals_id(id_type id) { 
            size_t count = 0;
            for(iterator iter = begin(id); iter != end(id); ++iter){
                if(*iter == 1)
                    count ++;
            }
            return count;
        }

        virtual void clear() {
            std::vector< std::vector<value_type> >().swap(adj_matrix);
        }

        virtual size_t _sizeof() {
            return sizeof(value_type) * num_ids() * num_ids();
        }

#ifdef STORAGE_H_DEBUG
        virtual void print() {
            std::cout << "Adjacent Matrix: " << std::endl;
            for (size_t i = 0; i < adj_matrix.size(); ++i) {
                for(size_t j = 0; j < adj_matrix[i].size(); ++j) {
                    std::cout << adj_matrix[i][j] << " ";
                }
                std::cout << std::endl;
            }
        }
#endif

    private:
        std::vector< std::vector<value_type> > adj_matrix;
};

#endif
