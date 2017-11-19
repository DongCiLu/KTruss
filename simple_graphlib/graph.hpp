/*
 * Component: Graph abstraction
 * Author: Zheng Lu
 * Email: zlu12@utk.edu
 * Note: This is a header only class
 */

#ifndef CS594_GRAPH_H
#define CS594_GRAPH_H

#include <map>
#include <set>
#include <list>
#include <iterator>

#include "storage.hpp"

template<typename id_type, typename v_data_type = int, typename e_data_type = int>
class graph {
    public:
        typedef id_type vertex_id_type;
        typedef v_data_type vertex_data_type;
        typedef e_data_type edge_data_type;

        graph(storage<vertex_id_type> *out, 
                storage<vertex_id_type> *in, 
                std::map<vertex_id_type, vertex_data_type> *vdata = NULL,
                std::map<std::pair<vertex_id_type, vertex_id_type>, 
                edge_data_type> *edata = NULL,
                bool directed = false) : 
            out(out), in(in), vdata(vdata), edata(edata), directed(directed) { }
        
        inline size_t num_vertices() {
            return std::max(out->num_ids(), in->num_ids());
        }

        inline size_t num_edges() {
            return out->num_vals();
        }

        inline size_t get_boundary(vertex_id_type id) {
            return out->num_vals_id(id);
        }

        inline size_t get_deg(vertex_id_type id) {
            return out->num_vals_id(id) + in->num_vals_id(id);
        }

        inline size_t get_out_deg(vertex_id_type id) {
            return directed ? out->num_vals_id(id) : 
                (out->num_vals_id(id) + in->num_vals_id(id));
        }

        inline size_t get_in_deg(vertex_id_type id) {
            return directed ? in->num_vals_id(id) : 
                (out->num_vals_id(id) + in->num_vals_id(id));
        }

        // out edge 0 - n-1, in edge 0 - n-1
        inline vertex_id_type get_nbr(vertex_id_type id, size_t i) {
            size_t num_out_vals = out->num_vals_id(id);
            if (i < num_out_vals)
                return *(out->begin(id) + i);
            else
                return *(in->begin(id) + i - num_out_vals);
        }

        inline edge_data_type get_edge_data(
                std::pair<vertex_id_type, vertex_id_type> edge) {
            if (directed) {
                return (*edata)[edge];
            }
            else {
                if (edata->find(edge) != edata->end())
                    return (*edata)[edge];
                else
                    return (*edata)[std::make_pair(edge.second, edge.first)];
            }
        }

        inline void set_vertex_data(vertex_id_type id, vertex_data_type val) {
            (*vdata)[id] = val;
        }

        inline vertex_data_type get_vertex_data(vertex_id_type id) {
            return (*vdata)[id];
        }

        inline std::list< std::pair<vertex_id_type, vertex_id_type> > 
            get_edge_list() {
            std::list< std::pair<vertex_id_type, vertex_id_type> > edge_list;
            for (vertex_id_type id = 0; id < out->num_ids(); ++id) {
                for (typename storage<vertex_id_type>::iterator 
                        iter = out->begin(id); iter != out->end(id); ++iter) 
                    edge_list.push_back(std::make_pair(id, *iter));
            }
            return edge_list;
        }

    private:
        storage<vertex_id_type> *out; // for fast access out edges
        storage<vertex_id_type> *in; // tranverse of r, for in edges
        std::map<vertex_id_type, vertex_data_type> *vdata; // vertex data
        std::map<std::pair<vertex_id_type, vertex_id_type>, 
            edge_data_type> *edata; // edge data
        bool directed;
};

#endif
