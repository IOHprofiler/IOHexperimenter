#pragma once

#include "graph_problem.hpp"

namespace ioh::problem
{
    namespace submodular
    {
        namespace pwt
        {
            //! Helpers for PackWhileTravel
            struct Point
            {
                int x, y;

                double distance(const Point &other) const
                {
                    return std::ceil(std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2)));
                }
            };

            struct Item
            {
                int index, profit, weight, node_number;
            };

            //! TODO: Find out what data is redundant and how we can better map to other graph data
            //! TODO: Don't use stringstream; slow
            struct TTPData
            {
                std::string name;
                std::string data_type;
                int dimension;
                int n_items;
                int capacity;
                double min_speed;
                double max_speed;
                double rent_ratio;
                std::string edge_weight_type;

                double penalty = 0;
                double velocity_gap = 0;
                std::vector<double> distances{};
                std::vector<Point> nodes{};
                std::vector<Item> items{};
                std::map<size_t, std::vector<Item>> city_map{};
            };

            struct TTPGraph : graph::Graph
            {
                using Graph::Graph;

                TTPData ttp_data;

                [[nodiscard]] std::string after_colon(const std::string &line) const
                {
                    std::stringstream ss(line);
                    std::string result;
                    std::getline(ss, result, ':');
                    std::getline(ss, result, ':');
                    common::trim(result);
                    return result;
                }


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

            struct PWTConstraint : Constraint<int>
            {
                std::shared_ptr<TTPGraph> graph;

                PWTConstraint(const std::shared_ptr<TTPGraph> &graph) :
                    Constraint(constraint::Enforced::HARD), graph(graph)
                {
                }
                
                bool compute_violation(const std::vector<int> &x) override
                { 
                    violation_ = 0; 
                    
                    for (size_t i = 0; i < graph->ttp_data.distances.size(); i++)
                        for (auto &item : graph->ttp_data.city_map[i])
                            violation_ += item.weight * static_cast<bool>(x[item.index]);
                    
                    return violation_ > graph->ttp_data.capacity;
                }

                double penalty() const override {
                    return weight * pow(graph->ttp_data.capacity - violation_ + graph->ttp_data.penalty, exponent);   
                }

                std::string repr() const override { return fmt::format("<PWTConstraint {}>", violation()); }
            };
        } // namespace pwt


        struct PackWhileTravel final : GraphProblemType<PackWhileTravel>
        {
            static inline int default_id = 2300;
            /**
             * @brief Construct a new PackWhileTravel object. Suggested usage is via the factory.
             * If you want to create your own objects, please be sure to pass a correct graph instance.
             *
             * @param problem_id the id to the problem
             * @param instance_id for instance based problems this is ignored
             * @param graph the graph object on which to operate
             */
            PackWhileTravel(const int problem_id, const int, const std::shared_ptr<pwt::TTPGraph> &graph) :
                GraphProblemType(problem_id, 1, fmt::format("PackWhileTravel{}", problem_id), graph)
            {
                constraintset_[0] = std::make_shared<pwt::PWTConstraint>(graph);
            }

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

    template <>
    inline InstanceBasedProblem::Constructors<submodular::PackWhileTravel, int, int>
    InstanceBasedProblem::load_instances<submodular::PackWhileTravel>(const std::optional<fs::path> &definitions_file)
    {
        using namespace submodular;
        return GraphProblemType<PackWhileTravel>::get_constructors<pwt::TTPGraph>(
            definitions_file.value_or(common::file::utils::find_static_file("example_list_pwt")));
    }
} // namespace ioh::problem
