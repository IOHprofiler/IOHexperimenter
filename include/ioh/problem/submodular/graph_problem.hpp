// Author: Viet Anh Do

#pragma once

#include <ioh/problem/problem.hpp>
#include <ioh/problem/transformation.hpp>
#include <ioh/common/log.hpp>

#include <fstream>
#include <limits>

#include "ioh/common/log.hpp"
#include "ioh/problem/problem.hpp"
#include "ioh/problem/transformation.hpp"

namespace ioh
{
    namespace problem
    {
        namespace submodular
        {
            class Helper
            {
            public:
                inline static std::string instance_list_path; // Default path to instance list file
                // Helper: Get end of line characters in a file, assume file can be read
                static char get_eol_in_file(const std::string &file_path)
                {
                    std::ifstream file_data(file_path);
                    char eol = 10; // Unix, Windows
                    std::string test;
                    std::getline(file_data, test, eol); // Attempt to read next line
                    auto index = test.find_first_of(13); // MacOS
                    if (index != std::string::npos) // Check if line break is passed
                        eol = 13;
                    return eol;
                }
                // Helper: Read instance list from file
                static std::vector<std::string> read_list_instance(const std::string &path_to_meta_list_instance)
                {
                    std::vector<std::string> l{};
                    std::ifstream list_data(path_to_meta_list_instance);
                    if (!list_data)
                    {
                        IOH_DBG(warning, fmt::format("Fail to instance list file: {}", path_to_meta_list_instance))
                        return {};
                    }
                    instance_list_path = path_to_meta_list_instance;
                    char eol = get_eol_in_file(path_to_meta_list_instance);
                    std::string str{};
                    while (std::getline(list_data, str, eol))
                    {
                        if (str.empty()) // Skip over empty lines
                            continue;
                        l.push_back(str);
                    }
                    return l;
                }
                // Helper: check if string is double
                static bool is_double(const std::string &text, double *num = new double())
                {
                    return ((std::istringstream(text) >> *num >> std::ws).eof());
                }
                // Helper: check if string is integer
                static bool is_int(const std::string &text, int *num = new int())
                {
                    return ((std::istringstream(text) >> *num >> std::ws).eof());
                }
            };

            class GraphInstance
            {
            protected:
                bool is_digraph = false;
                int n_vertices = 0, n_edges = 0;
                double cons_weight_limit = std::numeric_limits<double>::infinity(); // Weight constraint
                std::vector<std::vector<int>>
                    adj_array{}; // Adjacency array for vertices, outgoing edges only if digraph
                std::vector<std::vector<double>>
                    edge_weights{}; // Edge weights in adjacency array form, must be paired with adj_array
                std::vector<double> vertex_weights{}; // Vertex weights for objective function
                std::vector<double> cons_weights{}; // Vertex weights for constraint function
                double chance_cons_factor =
                    0; // chance constraint factor c, to be added to constraint value as c*sqrt(|x|)
            public:
                // Getters
                bool get_is_digraph() const { return is_digraph; }
                int get_n_vertices() const { return n_vertices; }
                int get_n_edges() const { return n_edges; }
                double get_cons_weight_limit() const { return cons_weight_limit; }
                int get_adj_vertex(const int vertex, const int neighbor) const { return adj_array[vertex][neighbor]; }
                std::vector<int> const &get_neighbors(const int vertex) const { return adj_array[vertex]; }
                double get_edge_weight(const int in_vertex, const int out_neighbor_index) const
                {
                    return edge_weights[in_vertex][out_neighbor_index];
                }
                double get_vertex_weight(const int vertex) const { return vertex_weights[vertex]; }
                double get_cons_weight(const int index) const { return cons_weights[index]; }
                int get_cons_weights_count() const { return cons_weights.size(); }
                double get_chance_cons_factor() const { return chance_cons_factor; }

                // Check if graph is empty
                bool is_empty() { return n_vertices == 0; }

                /**
                 * @brief Read a graph object with optional weights
                 *
                 * @param edge_file name of file containing edge, one per line, starting with digraph flag
                 * @param e_weights name of file containing edge weights, one per line
                 * @param v_weights name of file containing vertex weights, one per line
                 * @param c_weights name of file containing constraint weights for knapsack constraint, one per line,
                 * @param chance_cons chance constraint factor, default to 0
                 * can be for edges for vertices, default to vertices
                 */
                GraphInstance(const std::string &edge_file = "NULL", const std::string &e_weights = "NULL",
                              const std::string &v_weights = "NULL", const std::string &c_weights = "NULL",
                              const bool is_edge = false, const double chance_cons = 0)
                {
                    // Read edge data (adjacency)
                    if (edge_file == "NULL")
                    {
                        return;
                    }
                    std::ifstream edge_data(edge_file);
                    if (!edge_data)
                        throw std::invalid_argument("Fail to open edge_file: " + (edge_file));
                    char eol = Helper::get_eol_in_file(edge_file);
                    std::string str;
                    std::getline(edge_data, str, eol);
                    if (str == "1")
                    {
                        is_digraph = true;
                    }
                    std::vector<std::vector<int>> edge_indexes{};
                    while (std::getline(edge_data, str, eol))
                    {
                        if (str.empty()) // Skip over empty lines
                            continue;
                        int first_vertex = 0, second_vertex = 1;
                        for (size_t i = 0; i < str.size(); i++)
                        {
                            if (str[i] == ' ') // Read 2 end vertices
                            {
                                if (Helper::is_int(str.substr(0, i), &first_vertex) &&
                                    Helper::is_int(str.substr(i + 1), &second_vertex))
                                {
                                    first_vertex--;
                                    second_vertex--;
                                }
                                else
                                {
                                    IOH_DBG(warning, "Invalid edge file format" ); // FIXME raise an exception?
                                    n_vertices=0;
                                    return;
                                }
                                break;
                            }
                        }
                        int max_vertex = std::max(first_vertex, second_vertex);
                        while (static_cast<int>(adj_array.size()) <= max_vertex) // Fill array until enough entries for vertices
                        {
                            adj_array.push_back({});
                            edge_weights.push_back({});
                            vertex_weights.push_back(1);
                        }
                        edge_indexes.push_back({first_vertex, (int)adj_array[first_vertex].size()});
                        adj_array[first_vertex].push_back(second_vertex - 1);
                        edge_weights[first_vertex].push_back(1); // Default value
                        if (!is_digraph)
                        {
                            adj_array[second_vertex].push_back(first_vertex);
                            edge_weights[second_vertex].push_back(1); // Default value
                        }
                        n_edges++;
                    }
                    n_vertices = adj_array.size();

                    // Read edge weights data
                    // if (!e_weights->empty())
                    if (e_weights != "NULL")
                    {
                        int index = 0;
                        std::ifstream e_weights_data(e_weights);
                        if (!e_weights_data)
                            throw std::invalid_argument("Fail to open e_weights: " + e_weights);
                        eol = Helper::get_eol_in_file(e_weights);
                        while (std::getline(e_weights_data, str, eol))
                        {
                            if (str.empty()) // Skip over empty lines
                                continue;
                            double temp;
                            if (Helper::is_double(str, &temp))
                                edge_weights[edge_indexes[index][0]][edge_indexes[index][1]] = temp;
                            else
                            {
                                IOH_DBG(warning, "Invalid edge weights file format" ); // FIXME raise an exception?
                                n_vertices = 0;
                                return;
                            }
                            index++;
                        }
                    }

                    // Read vertex weights data
                    // if (!v_weights->empty())
                    if (v_weights != "NULL")
                    {
                        int index = 0;
                        std::ifstream v_weights_data(v_weights);
                        if (!v_weights_data)
                            throw std::invalid_argument("Fail to open v_weights: " + v_weights);
                        eol = Helper::get_eol_in_file(v_weights);
                        while (std::getline(v_weights_data, str, eol))
                        {
                            if (str.empty()) // Skip over empty lines
                                continue;
                            double temp;
                            if (Helper::is_double(str, &temp))
                                vertex_weights[index++] = temp;
                            else
                            {
                                IOH_DBG(warning, "Invalid vertex weights file format" ); // FIXME raise an exception?
                                n_vertices = 0;
                                return;
                            }
                        }
                    }

                    // Read constraint weights data (include bound at last line)
                    // if (c_weights->empty())
                    if (c_weights == "NULL")
                        cons_weights = std::vector<double>(is_edge ? n_edges : n_vertices, 1);
                    else
                    {
                        std::ifstream c_weights_data(c_weights);
                        if (!c_weights_data)
                            throw std::invalid_argument("Fail to open c_weights: " + c_weights);
                        eol = Helper::get_eol_in_file(c_weights);
                        while (std::getline(c_weights_data, str, eol))
                        {
                            if (str.empty()) // Skip over empty lines
                                continue;
                            double temp;
                            if (Helper::is_double(str, &temp))
                                cons_weights.push_back(temp);
                            else
                            {
                                IOH_DBG(warning, "Invalid constraint weights file format" ); // FIXME raise an exception?
                                n_vertices = 0;
                                return;
                            }
                        }
                        cons_weight_limit = cons_weights.back();
                        cons_weights.pop_back();
                    }
                    // Assign chance constraint factor
                    chance_cons_factor = chance_cons;
                }
                // Instantiate graph instance object via file names vector
                GraphInstance(const std::vector<std::string> &files = {"NULL"}, const bool is_edge = false) :
                    GraphInstance(files[0], files.size() > 1 ? files[1] : nullptr,
                                  files.size() > 2 ? files[2] : nullptr, files.size() > 3 ? files[3] : nullptr, is_edge,
                                  (files.size() > 4 && Helper::is_double(files[4])) ? std::stod(files[4]) : 0)
                {
                }
                // Empty constructor
                GraphInstance() {}
            };
            //! Graph base class
            class Graph : public Integer
            {
            private:
            protected:
                GraphInstance *graph;
                bool is_initialized;
                //! Variables transformation method
                std::vector<int> transform_variables(std::vector<int> x) override { return x; }
                //! Objectives transformation method
                double transform_objectives(const double y) override { return y; }

            public:
                // Read list of graph instances to load, return problem dimension
                // First, read the meta list of files to load (one entry per line), then read the files
                // Each entry is formatted with {Edge list}|[Edge weights]|[Vertex weights]|[Constraint weights]|[Chance
                // constraint factor] "NULL" within an entry means no file Chance constraint factor must be in numeric
                int read_instances_from_files(const int instance, const bool is_edge = false,
                                              const std::string &path_to_meta_list_graph = Helper::instance_list_path)
                {
                    std::ifstream list_data(path_to_meta_list_graph);
                    if (!list_data)
                    {
                        IOH_DBG(warning, "Fail to open instance list file: " << path_to_meta_list_graph ); // FIXME raise an exception?
                        is_initialized = false;
                        return 0;
                    }
                    char eol = Helper::get_eol_in_file(path_to_meta_list_graph);
                    std::string str{};
                    auto counter = 0;
                    while (std::getline(list_data, str, eol))
                    {
                        if (str.empty()) // Skip over empty lines
                            continue;
                        if (counter++ == instance)
                        {
                            std::vector<std::string> entry(5, "NULL");
                            std::string rstr;
                            std::istringstream iss(str);
                            int index = 0;
                            while (std::getline(iss, rstr, '|') && index < 5) // File paths are delimited by '|'
                            {
                                entry[index++] = rstr.c_str();
                            }
                            graph = new GraphInstance(entry, is_edge); // Initialize graph instance object
                            is_initialized = true;
                            return is_edge ? graph->get_n_edges() : graph->get_n_vertices();
                        }
                    }
                    is_initialized = false;
                    return 0; // No matching instance (e.g. invalid instance)
                }
                // Check if instance is null
                bool is_null() { return (!is_initialized) || graph->is_empty(); }

                // Get problem dimension from initialized graph instance
                int get_dim(const bool is_edge = false)
                {
                    return is_null() ? 0 : is_edge ? graph->get_n_edges() : graph->get_n_vertices();
                }

                /**
                 * @brief Construct a new Graph object
                 *
                 * @param problem_id The id of the problem
                 * @param instance The instance of the problem
                 * @param name the name of the problem
                 * @param is_edge whether to define problem dimension with edges or vertices
                 * @param instance_list_file the name of file containing instance list
                 */
                Graph(const int problem_id, const int instance, const int n_variables, const std::string &name,
                      [[maybe_unused]] const bool is_edge, [[maybe_unused]] const std::string &instance_list_file = Helper::instance_list_path) :
                    Integer(MetaData(problem_id, instance, name, n_variables,
                                     common::OptimizationType::Maximization),
                            Constraint<int>(n_variables, 0, 1))
                {
                    if (is_null())
                    {
                        IOH_DBG(warning, "Invalid instance id. Skip creating instance oracle." ); // FIXME raise an exception?
                        return;
                    }
                }
            };

            /**
             * @brief CRTP class for Graph problems. Inherit from this class when defining new Graph problems
             *
             * @tparam ProblemType The New Graph problem class
             */
            template <typename ProblemType>
            class GraphProblem : public Graph,
                                 AutomaticProblemRegistration<ProblemType, Graph>,
                                 AutomaticProblemRegistration<ProblemType, Integer>
            {
            public:
                using Graph::Graph;
            };
        } // namespace submodular
    } // namespace problem
} // namespace ioh
