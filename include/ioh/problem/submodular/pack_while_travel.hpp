#pragma once

#include "graph_problem.hpp"

namespace ioh::problem
{
    namespace submodular
    {
        namespace pwt
        {
            //! Abstraction of a point in xy space
            struct Point
            {
                //! X Coordinate
                int x;
                //! Y Coordinates
                int y;
                
                /**
                 * @brief Compute Euclidian distance to other point
                 * 
                 * @param other Another point
                 * @return double Euclidian distance
                 */
                [[nodiscard]] double distance(const Point &other) const
                {
                    return std::ceil(std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2)));
                }
            };

            //! Meta data container for items in the grpah
            struct Item
            {
                //! Numeric index
                int index;
                
                //! ammount
                int profit;
                
                //! Weight
                int weight;

                //! node idex
                int node_number;
            };

            //! TODO: Find out what data is redundant and how we can better map to other graph data
            //! TODO: Don't use stringstream; slow
            //! Raw TTP-Graph data container
            struct TTPData
            {
                //! The name
                std::string name;

                //! The data type
                std::string data_type;

                //! The dimension
                int dimension;

                //! The number of items
                int n_items;

                //! The capacity
                int capacity;

                //! The min speed
                double min_speed;

                //! The max speed
                double max_speed;

                //! Rent ration
                double rent_ratio;

                //! The type weight
                std::string edge_weight_type;

                //! numeric constant penalty
                double penalty = 0;
                
                //! Veclocity gap
                double velocity_gap = 0;

                //! Vector of distances
                std::vector<double> distances{};

                //! Vector of nodes (points)
                std::vector<Point> nodes{};

                //! Vector of items (Item)
                std::vector<Item> items{};

                //! Map of index to Item for fast access
                std::map<size_t, std::vector<Item>> city_map{};
            };
            
            //! Graph data container for pwt
            struct TTPGraph : graph::Graph
            {
                using Graph::Graph;
                
                //! Dataset containing the raw data
                TTPData ttp_data;

                /**
                 * @brief Split a string after a colon
                 * 
                 * @param line the line to split from
                 * @return std::string the thing after the colon
                 */
                [[nodiscard]] std::string after_colon(const std::string &line) const
                {
                    std::stringstream ss(line);
                    std::string result;
                    std::getline(ss, result, ':');
                    std::getline(ss, result, ':');
                    common::trim(result);
                    return result;
                }

                //! Load the graph from the static files
                void load() override
                {
                    const auto contents = common::file::as_text_vector(meta.root / meta.edge_file);

                    ttp_data = {after_colon(contents[0]),
                                after_colon(contents[1]),
                                std::stoi(after_colon(contents[2])),
                                std::stoi(after_colon(contents[3])),
                                std::stoi(after_colon(contents[4])),
                                std::stod(after_colon(contents[5])),
                                std::stod(after_colon(contents[6])),
                                std::stod(after_colon(contents[7])),
                                after_colon(contents[8])};

                    ttp_data.nodes.resize(ttp_data.dimension);

                    const auto coords_until = static_cast<size_t>(ttp_data.dimension) + 10;

                    std::istringstream stream;
                    for (size_t i = 10; i < coords_until; i++)
                    {
                        const auto current = i - 10;
                        stream.str(contents[i]);
                        stream >> ttp_data.nodes[current].x >> ttp_data.nodes[current].x >> ttp_data.nodes[current].y;

                        if (current > 0)
                        {
                            ttp_data.distances.push_back(ttp_data.nodes[current].distance(ttp_data.nodes[current - 1]));
                            ttp_data.penalty -= ttp_data.distances.back();
                        }
                        stream.clear();
                    }
                    ttp_data.velocity_gap = ttp_data.max_speed - ttp_data.min_speed;
                    ttp_data.distances.push_back(ttp_data.nodes.front().distance(ttp_data.nodes.back()));
                    ttp_data.penalty = (ttp_data.penalty - ttp_data.distances.back()) * ttp_data.rent_ratio /
                        (ttp_data.max_speed - ttp_data.velocity_gap);

                    ttp_data.items.resize(ttp_data.n_items);
                    for (size_t i = (coords_until + 1); i < contents.size(); i++)
                    {
                        const auto index = i - (coords_until + 1);
                        auto &record = ttp_data.items[index];
                        std::stringstream{contents[i]} >> record.index >> record.profit >> record.weight >>
                            record.node_number;
                        record.node_number--;
                        record.index--;
                        ttp_data.city_map[record.node_number].push_back(record);
                    }
                    meta.is_edge = false;
                    meta.n_vertices = ttp_data.n_items;

                    IOH_DBG(xdebug, "loaded pwt graph problem");
                    IOH_DBG(xdebug, "number of vertices: " << meta.n_vertices);
                    IOH_DBG(xdebug, "number of items: " << ttp_data.items.size());
                    loaded = true;
                }
            };
            
            //! Graph contraint for pwt
            struct PWTConstraint : Constraint<int>
            {
                //! pointer to the graph data
                std::shared_ptr<TTPGraph> graph;
                /**
                 * @brief Construct a new PWTConstraint object
                 * 
                 * @param graph a shared_ptr to some TTPGraph
                 */
                PWTConstraint(const std::shared_ptr<TTPGraph> &graph) :
                    Constraint(constraint::Enforced::HARD), graph(graph)
                {
                }
                /**
                 * @brief Compute constraint violation
                 * 
                 * @param x the candidate solution
                 * @return true when there is constraint violation
                 * @return false when there is no constraint violaton
                 */
                bool compute_violation(const std::vector<int> &x) override
                { 
                    violation_ = 0; 
                    
                    for (size_t i = 0; i < graph->ttp_data.distances.size(); i++)
                        for (auto &item : graph->ttp_data.city_map[i])
                            violation_ += item.weight * static_cast<bool>(x[item.index]);
                    
                    return violation_ > graph->ttp_data.capacity;
                }

                //! Penalty function, scales violation
                [[nodiscard]] double penalty() const override {
                    return weight * pow(graph->ttp_data.capacity - violation_ + graph->ttp_data.penalty, exponent);   
                }
                
                //! String representation
                [[nodiscard]] std::string repr() const override { return fmt::format("<PWTConstraint {}>", violation()); }
            };
        } // namespace pwt

        //! PWT problem definition
        struct PackWhileTravel final : GraphProblemType<PackWhileTravel>
        {
            //! PackWhileTravel problems start with problem ids starting from this id
            static inline int default_id = 2300;
            /**
             * @brief Construct a new PackWhileTravel object. Suggested usage is via the factory.
             * If you want to create your own objects, please be sure to pass a correct graph instance.
             *
             * @param problem_id the id to the problem
             * @param graph the graph object on which to operate
             */
            PackWhileTravel(const int problem_id, const int, const std::shared_ptr<pwt::TTPGraph> &graph) :
                GraphProblemType(problem_id, 1, fmt::format("PackWhileTravel{}", problem_id), graph)
            {
                constraintset_[0] = std::make_shared<pwt::PWTConstraint>(graph);
            }

            /**
             * @brief Evaluate the problem
             * 
             * @param x the candidate solution
             * @return double the raw problem value
             */
            double evaluate(const std::vector<int> &x) override
            {
                auto& data = std::dynamic_pointer_cast<pwt::TTPGraph>(graph)->ttp_data;
                auto profit_sum = 0.0, cons = 0.0, time = 0.0;

                for (size_t i = 0; i < data.distances.size(); i++)
                {
                    for (auto &item : data.city_map[i])
                    {
                        if (x[item.index] >= 1)
                        {
                            cons += item.weight;
                            profit_sum += item.profit;
                        }
                    }
                    time += data.distances[i] / (data.max_speed - (data.velocity_gap * cons / data.capacity));
                }          
                return profit_sum - time;
            }
        };
    } // namespace submodular

    /**
     * @brief Template instantiation for pwt problems.
     * Ensures loading of PWT problems in the factory, using the correct files.
     * 
     * @param definitions_file optional file name
     * @return InstanceBasedProblem::Constructors<submodular::PackWhileTravel, int, int> a vector of contructor functions
     */
    template <>
    inline InstanceBasedProblem::Constructors<submodular::PackWhileTravel, int, int>
    InstanceBasedProblem::load_instances<submodular::PackWhileTravel>(const std::optional<fs::path> &definitions_file)
    {
        using namespace submodular;
        return GraphProblemType<PackWhileTravel>::get_constructors<pwt::TTPGraph>(
            definitions_file.value_or(common::file::utils::find_static_file("example_list_pwt")));
    }
} // namespace ioh::problem
