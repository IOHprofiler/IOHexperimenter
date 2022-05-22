#pragma once

#include <ioh/common.hpp>
#include <ioh/problem/problem.hpp>
#include <queue>

namespace ioh::problem::submodular
{
    namespace v2
    {

        namespace graph
        {
            struct Meta
            {
                std::string edge_file;
                std::string edge_weights;
                std::string vertex_weights;
                std::string constraint_weights;
                fs::path root;
                bool is_edge = false;
                double chance_cons = 0.0;

                bool digraph = false;
                double constraint_limit = std::numeric_limits<double>::infinity();
                int n_vertices = 0;

                static Meta from_string(const std::string &definition)
                {
                    Meta meta;
                    std::string dp;
                    std::array<std::string *, 5> p{&meta.edge_file, &meta.edge_weights, &meta.vertex_weights,
                                                   &meta.constraint_weights, &dp};
                    std::stringstream ss(definition);
                    size_t i = 0;
                    while (getline(ss, *p[i++], '|') && (i < p.size()))
                        ;
                    try
                    {
                        meta.chance_cons = std::stod(dp);
                    }
                    catch (...)
                    {
                        meta.chance_cons = 0.0;
                    }
                    // default path for all files, change this in the if you want to use another
                    meta.root = common::file::utils::get_static_root();
                    return meta;
                }
            };

            struct Graph : ioh::common::HasRepr
            {
                Meta meta;
                std::vector<std::vector<std::pair<int, double>>> adjacency_list;
                std::vector<std::tuple<int, int, double>> edges;
                std::vector<double> edge_weights;
                std::vector<double> vertex_weights;
                std::vector<double> constraint_weights;


                bool loaded = false;

                Graph(const Meta &meta) : meta(meta) {}

                Graph(const std::string &definition) : Graph(Meta::from_string(definition)) {}

                std::string repr() const
                {
                    return fmt::format("<GraphInstance {} {} {} {}>", meta.edge_file, meta.edge_weights,
                                       meta.vertex_weights, meta.constraint_weights);
                }

                void load()
                {
                    const auto contents = ioh::common::file::as_text_vector(meta.root / meta.edge_file);
                    meta.digraph = static_cast<bool>(std::stoi(contents[0]));

                    if (!meta.edge_weights.empty() && meta.edge_weights != "NULL")
                        edge_weights = ioh::common::file::as_numeric_vector<double>(meta.root / meta.edge_weights);
                    else
                        edge_weights = std::vector<double>(contents.size() - 1, 1.0);

                    for (size_t i = 1; i < contents.size(); i++)
                    {
                        int source, target;
                        std::stringstream{contents[i]} >> source >> target;
                        meta.n_vertices = std::max(meta.n_vertices, std::max(source, target));
                        edges.push_back(std::make_tuple(source - 1, target - 1, edge_weights[i - 1]));
                    }

                    adjacency_list.resize(meta.n_vertices);
                    for (const auto &[source, target, weight] : edges)
                    {
                        adjacency_list[source].push_back({target, weight});
                        if (!meta.digraph)
                            adjacency_list[target].push_back({source, weight});
                    }

                    if (!meta.vertex_weights.empty() && meta.vertex_weights != "NULL")
                        vertex_weights = ioh::common::file::as_numeric_vector<double>(meta.root / meta.vertex_weights);
                    else
                        vertex_weights = std::vector<double>(meta.n_vertices, 1.);

                    if (!meta.constraint_weights.empty() && meta.constraint_weights != "NULL")
                    {
                        constraint_weights =
                            ioh::common::file::as_numeric_vector<double>(meta.root / meta.constraint_weights);
                        meta.constraint_limit = constraint_weights.back();
                        constraint_weights.pop_back();
                    }
                    else
                    {
                        constraint_weights = std::vector<double>(meta.is_edge ? edges.size() : meta.n_vertices, 1);
                    }

                    IOH_DBG(xdebug, "loaded graph problem");
                    IOH_DBG(xdebug, "number of edges:  " << edges.size());
                    IOH_DBG(xdebug, "number of vertices: " << meta.n_vertices);
                    IOH_DBG(xdebug, "number of edge_weights: " << edge_weights.size());
                    IOH_DBG(xdebug, "number of vertex_weights: " << vertex_weights.size());
                    IOH_DBG(xdebug, "number of constraints: " << constraint_weights.size());
                    loaded = true;
                }

                int dimension()
                {
                    if (!loaded)
                        load();
                    return meta.is_edge ? static_cast<int>(edges.size()) : meta.n_vertices;
                }
            };
        } // namespace graph


        struct GraphProblem : Integer
        {
            graph::Graph graph;

            GraphProblem(const int problem_id, const int instance, const std::string &name, graph::Graph graph) :
                Integer(MetaData(problem_id, instance, name, graph.dimension(), common::OptimizationType::Maximization),
                        Constraint<int>(graph.dimension(), 0, 1)),
                graph(graph)
            {
            }
        };


        template <typename ProblemType>
        struct GraphProblemType : GraphProblem,
                                  InstanceBasedProblem,
                                  AutomaticProblemRegistration<ProblemType, Integer>,
                                  AutomaticProblemRegistration<ProblemType, GraphProblem>
        {

            using GraphProblem::GraphProblem;

            /**
             * @brief Get the constructors used in explicit specialization of load_isntances method if
             * InstanceBasedProblem
             *
             * @param path
             * @return InstanceBasedProblem::Constructors<ProblemType, int, int>
             */
            static InstanceBasedProblem::Constructors<ProblemType, int, int> get_constructors(const fs::path &path)
            {
                InstanceBasedProblem::Constructors<ProblemType, int, int> constructors;

                auto root_path = path.parent_path();
                auto graphs = common::file::as_text_vector<graph::Graph>(path);
                int i = ProblemType::default_id;
                for (auto graph : graphs)
                {
                    graph.meta.root = root_path;
                    constructors.push_back({[graph, i](int, int) { 
                        return ProblemType(i, 1, graph); }, i++});
                }
                return constructors;
            }
        };

        namespace problems
        {

            struct MaxCut : GraphProblemType<MaxCut>
            {
                static inline int default_id = 2000;

                /**
                 * @brief Construct a new MaxCut object. Suggested usage is via the factory.
                 * If you want to create your own objects, please be sure to pass a correct graph instance.
                 *
                 * @param problem_id the id to the problem
                 * @param instance_id for instance based problems this is ignored
                 * @param graph the graph object on which to operate
                 */
                MaxCut(const int problem_id, const int, graph::Graph graph) :
                    GraphProblemType(problem_id, 1, fmt::format("MaxCut{}", problem_id), graph)
                {
                    objective_.x = std::vector<int>(graph.dimension(), 1);
                    objective_.y = evaluate(objective_.x);
                }

                double evaluate(const std::vector<int> &x) override
                {
                    double result = 0, constraint = 0;
                    int count = 0;
                    for (size_t source = 0; source < x.size(); source++)
                    {
                        if (x[source])
                        {
                            constraint += graph.constraint_weights[source];
                            count++;
                            for (const auto &[target, weight] : graph.adjacency_list[source])
                                result += weight * (x[target] == 0);
                        }
                        else if (graph.meta.digraph)
                        {
                            for (const auto &[target, weight] : graph.adjacency_list[source])
                                result -= weight * (x[target] == 1);
                        }
                    }
                    constraint += sqrt(count) * graph.meta.chance_cons;
                    if (constraint > graph.meta.constraint_limit)
                        result = graph.meta.constraint_limit - constraint;
                    return result;
                }
            };

            struct MaxCoverage : GraphProblemType<MaxCoverage>
            {
                static inline int default_id = 2100;
                std::vector<uint8_t> is_covered;
                /**
                 * @brief Construct a new MaxCoverage object. Suggested usage is via the factory.
                 * If you want to create your own objects, please be sure to pass a correct graph instance.
                 *
                 * @param problem_id the id to the problem
                 * @param instance_id for instance based problems this is ignored
                 * @param graph the graph object on which to operate
                 */
                MaxCoverage(const int problem_id, const int, graph::Graph graph) :
                    GraphProblemType(problem_id, 1, fmt::format("MaxCoverage{}", problem_id), graph),
                    is_covered(std::vector<uint8_t>(graph.dimension(), 0))
                {
                    objective_.x = std::vector<int>(graph.dimension(), 1);
                    objective_.y = evaluate(objective_.x);
                }

                double evaluate(const std::vector<int> &x) override
                {
                    std::fill(is_covered.begin(), is_covered.end(), 0);

                    double result = 0, constraint = 0;
                    int count = 0;
                    for (size_t source = 0; source < x.size(); source++)
                    {
                        if (x[source])
                        {
                            constraint += graph.constraint_weights[source];
                            count++;

                            if (!is_covered[source])
                            {
                                result += graph.vertex_weights[source];
                                is_covered[source] = 1;
                            }
                            for (const auto &[target, weight] : graph.adjacency_list[source])
                            {
                                if (!is_covered[target])
                                {
                                    result += graph.vertex_weights[target];
                                    is_covered[target] = 1;
                                }
                            }
                        }
                    }
                    constraint += sqrt(count) * graph.meta.chance_cons;
                    if (constraint > graph.meta.constraint_limit)
                        result = graph.meta.constraint_limit - constraint;
                    return result;
                }
            };


            struct MaxInfluence : GraphProblemType<MaxInfluence>
            {
                static inline int default_id = 2200;
                int simulation_reps = 100;
                std::vector<uint8_t> is_activated;

                /**
                 * @brief Construct a new MaxInfluence object. Suggested usage is via the factory.
                 * If you want to create your own objects, please be sure to pass a correct graph instance.
                 *
                 * @param problem_id the id to the problem
                 * @param instance_id for instance based problems this is ignored
                 * @param graph the graph object on which to operate
                 */
                MaxInfluence(const int problem_id, const int, graph::Graph graph) :
                    GraphProblemType(problem_id, 1, fmt::format("MaxInfluence{}", problem_id), graph),
                    is_activated(std::vector<uint8_t>(graph.dimension(), 0))
                {
                    objective_.x = std::vector<int>(graph.dimension(), 1);
                    objective_.y = evaluate(objective_.x);
                }

                double random_spread_count(const std::vector<int> &x)
                {
                    using namespace ioh::common::random;
                    double total = 0;

                    std::queue<size_t> visits;

                    for (size_t i = 0; i < x.size(); i++)
                    {
                        is_activated[i] = x[i];
                        if (x[i] != 0)
                            visits.push(i);
                    }

                    while (!visits.empty())
                    {
                        size_t source = visits.front();
                        visits.pop();

                        for (const auto &[target, weight] : graph.adjacency_list[source])
                        {
                            const double r = ioh::common::random::real();
                            if (!is_activated[target] && r <= weight)
                            {
                                is_activated[target] = 1;
                                visits.push(target);
                                total += graph.vertex_weights[target];
                            }
                        }
                    }
                    return total;
                }

                double evaluate(const std::vector<int> &x) override
                {
                    double result = 0, constraint = 0;
                    int count = 0;
                    for (size_t source = 0; source < x.size(); source++)
                    {
                        constraint += graph.constraint_weights[source] * x[source];
                        result += graph.vertex_weights[source] * x[source];
                        count += x[source];
                    }

                    constraint += sqrt(count) * graph.meta.chance_cons;
                    if (constraint > graph.meta.constraint_limit)
                        return graph.meta.constraint_limit - constraint;

                    for (auto i = 0; i < simulation_reps; i++)
                        result += random_spread_count(x) / simulation_reps;
                    return result;
                }
            };
        } // namespace problems

    } // namespace v2
} // namespace ioh::problem::submodular


namespace ioh::problem
{

    template <>
    inline InstanceBasedProblem::Constructors<submodular::v2::problems::MaxCut, int, int>
    InstanceBasedProblem::load_instances<submodular::v2::problems::MaxCut>(
        const std::optional<fs::path> &definitions_file)
    {
        using namespace submodular::v2;
        return GraphProblemType<problems::MaxCut>::get_constructors(
            definitions_file.value_or(common::file::utils::find_static_file("example_list_maxcut")));
    }

    template <>
    inline InstanceBasedProblem::Constructors<submodular::v2::problems::MaxCoverage, int, int>
    InstanceBasedProblem::load_instances<submodular::v2::problems::MaxCoverage>(
        const std::optional<fs::path> &definitions_file)
    {
        using namespace submodular::v2;
        return GraphProblemType<problems::MaxCoverage>::get_constructors(
            definitions_file.value_or(common::file::utils::find_static_file("example_list_maxcoverage")));
    }

    template <>
    inline InstanceBasedProblem::Constructors<submodular::v2::problems::MaxInfluence, int, int>
    InstanceBasedProblem::load_instances<submodular::v2::problems::MaxInfluence>(
        const std::optional<fs::path> &definitions_file)
    {
        using namespace submodular::v2;
        return GraphProblemType<problems::MaxInfluence>::get_constructors(
            definitions_file.value_or(common::file::utils::find_static_file("example_list_maxinfluence")));
    }
} // namespace ioh::problem