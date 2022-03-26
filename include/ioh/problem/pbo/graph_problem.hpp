#pragma once

#include "ioh.hpp"
#include <fstream>
#include <limits>

namespace ioh::problem
{
    std::vector<std::vector<std::string>> meta_list_graph;
    class GraphInstance
    {
    protected:
        bool is_digraph = false;
        int n_vertices = 0, n_edges = 0;
        double cons_weight_limit = std::numeric_limits<double>::infinity(); // Weight constraint
        std::vector<std::vector<int>> adj_array{}; // Adjacency array for vertices, outgoing edges only if digraph
        std::vector<std::vector<double>>
            edge_weights{}; // Edge weights in adjacency array form, must be paired with adj_array
        std::vector<double> vertex_weights{}; // Vertex weights for objective function
        std::vector<double> cons_weights{}; // Vertex weights for constraint function
        double chance_cons_factor = 0; // chance constraint factor c, to be added to constraint value as c*sqrt(|x|)

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

        // Setters
        void set_chance_cons_factor(double new_factor) { chance_cons_factor = new_factor; }

        /**
         * @brief Read a graph object with optional weights
         *
         * @param edge_file name of file containing edge, one per line, starting with digraph flag
         * @param e_weights name of file containing edge weights, one per line
         * @param v_weights name of file containing vertex weights, one per line
         * @param c_weights name of file containing constraint weights for knapsack constraint, one per line, can be for
         * edges for vertices, default to vertices
         */
        GraphInstance(const std::string *edge_file, const std::string *e_weights = nullptr,
                      const std::string *v_weights = nullptr, const std::string *c_weights = nullptr)
        {
            // Read edge data (adjacency)
            if (!edge_file)
                return;
            std::ifstream edge_data((*edge_file));
            std::string str;
            std::getline(edge_data, str);
            if (str == "1")
            {
                is_digraph = true;
            }
            std::vector<std::vector<int>> edge_indexes{};
            while (std::getline(edge_data, str))
            {
                if (str.empty())// Skip over empty lines
                    continue;
                int first_vertex, second_vertex;
                for (int i = 0; i < str.size(); i++)
                {
                    if (str[i] == ' ')// Read 2 end vertices
                    {
                        first_vertex = std::stoi(str.substr(0, i));
                        second_vertex = std::stoi(str.substr(i + 1));
                        break;
                    }
                }
                int max_vertex = std::max(first_vertex, second_vertex);
                while (adj_array.size() < max_vertex)// Fill array until enough entries for vertices
                {
                    adj_array.push_back({});
                    edge_weights.push_back({});
                    vertex_weights.push_back(1);
                }
                edge_indexes.push_back({first_vertex - 1, (int)adj_array[first_vertex - 1].size()});
                adj_array[first_vertex - 1].push_back(second_vertex - 1);
                edge_weights[first_vertex - 1].push_back(1);// Default value
                if (!is_digraph)
                {
                    adj_array[second_vertex - 1].push_back(first_vertex - 1);
                    edge_weights[second_vertex - 1].push_back(1);// Default value
                }
                n_edges++;
            }
            n_vertices = adj_array.size();

            // Read edge weights data
            if (!e_weights->empty())
            {
                int index = 0;
                std::ifstream e_weights_data((*e_weights));
                while (std::getline(e_weights_data, str))
                {
                    if (str.empty()) // Skip over empty lines
                        continue;
                    edge_weights[edge_indexes[index][0]][edge_indexes[index][1]] = std::stod(str);
                    index++;
                }
            }

            // Read vertex weights data
            if (!v_weights->empty())
            {
                int index = 0;
                std::ifstream v_weights_data((*v_weights));
                while (std::getline(v_weights_data, str))
                {
                    if (str.empty()) // Skip over empty lines
                        continue;
                    vertex_weights[index++] = std::stod(str);
                }
            }

            // Read constraint weights data (include bound at last line)
            if (c_weights->empty())
                cons_weights = std::vector<double>(n_vertices, 1);
            else
            {
                std::ifstream c_weights_data((*c_weights));
                while (std::getline(c_weights_data, str))
                {
                    if (str.empty()) // Skip over empty lines
                        continue;
                    cons_weights.push_back(std::stod(str));
                }
                cons_weight_limit = cons_weights.back();
                cons_weights.pop_back();
            }
        }
        // Instantiate graph instance object via id
        GraphInstance(const int instance) :
            GraphInstance(&meta_list_graph[instance][0], &meta_list_graph[instance][1], &meta_list_graph[instance][2],
                          &meta_list_graph[instance][3])
        {
        }
    };
    std::vector<GraphInstance *> graph_list;

    // Read list of graph instances to load, one entry per line
    // Each entry is formatted with {Edge list}|[Edge weights]|[Vertex weights]|[Constraint weights]
    int read_meta_list_graph(const bool reread = false,
                             const std::string &path_to_meta_list_graph = std::filesystem::current_path().string() +
                                 "\\" + "example_list")
    {
        if (meta_list_graph.empty() || reread)// Only read if unread, or forced reread
        {
            std::vector<std::vector<std::string>> l{};
            std::vector<GraphInstance *> g{};
            std::ifstream list_data(path_to_meta_list_graph);
            std::string str{};
            while (std::getline(list_data, str))
            {
                if (str.empty())// Skip over empty lines
                    continue;
                std::vector<std::string> entry(4, "");
                std::string rstr;
                std::istringstream iss(str);
                int index = 0;
                while (std::getline(iss, rstr, '|'))// File paths are delimited by '|'
                {
                    entry[index++] = rstr.c_str();
                }
                l.push_back(entry);
                g.push_back(nullptr);
            }
            meta_list_graph = l;
            graph_list = g;
        }
        return meta_list_graph.size();
    }

    // Get dimensions from graph instances, and load these if not yet loaded
    std::vector<int> get_dimensions_from_ids(const std::vector<int> &instances, const bool is_edge = false, const bool reread = false)
    {// instances are 0-indexed
        int list_size = read_meta_list_graph();
        std::vector<int> dimensions{};
        for (auto id : instances)
        {
            if (id >= list_size)
                dimensions.push_back(1);// Instance not in meta list, return a valid dummy size
            else {
                if (graph_list[id] == nullptr || reread)// Only read if unread, or forced reread
                    graph_list[id] = new GraphInstance(id);
                dimensions.push_back(is_edge ? graph_list[id]->get_n_edges() : graph_list[id]->get_n_vertices());
            }
        }
        return dimensions;
    }

    //! Graph base class
    class Graph : public PBO
    {
    protected:
        const ioh::problem::GraphInstance *graph;
        //! Variables transformation method
        std::vector<int> transform_variables(std::vector<int> x) override { return x; }
        //! Objectives transformation method
        double transform_objectives(const double y) override { return y; }
    public:
        /**
         * @brief Construct a new Graph object
         *
         * @param problem_id The id of the problem
         * @param instance The instance of the problem
         * @param name the name of the problem
         */
        Graph(const int problem_id, const int instance, const int n_variables, const std::string &name) :
            PBO(problem_id, instance, n_variables, name), // Assuming graph list is already instantiated
            graph(graph_list.empty() || graph_list.size() < instance ? nullptr: graph_list[instance-1])
        {
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
                         AutomaticProblemRegistration<ProblemType, PBO>,
                         AutomaticProblemRegistration<ProblemType, Integer>
    {
    public:
        using Graph::Graph;
    };
}

