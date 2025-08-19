#pragma once

#include "ioh/common.hpp"
#include "ioh/problem/single.hpp"


namespace ioh::problem::submodular
{
    namespace graph
    {
        //! Graph meta data
        struct Meta
        {
            //! File with edge data
            std::string edge_file;

            //! file with edge weights
            std::string edge_weights;

            //! File with vertex weights
            std::string vertex_weights;

            //! File with constraint weights
            std::string constraint_weights;

            // added by saba
            //! File with constraint variances
            std::string constraint_variances;

            //! The root path of the files
            fs::path root;

            //! Edge type
            bool is_edge = false;

            //! Fixed chance constraint
            // deleted by saba
            //  double chance_cons = 0.0;

            //! Is digraph
            bool digraph = false;

            //! Maximum value of the constraint
            double constraint_limit = std::numeric_limits<double>::infinity();

            //! The number of vertices
            int n_vertices = 0;

            //! Load a graph from a string
            static Meta from_string(const std::string &definition)
            {
                Meta meta;

                // changed by saba-----------------------------------------------------------------------------
                //  std::string dp;
                //  std::array<std::string *, 5> p{&meta.edge_file, &meta.edge_weights, &meta.vertex_weights,
                //                                 &meta.constraint_weights, &dp};

                std::array<std::string *, 5> p{&meta.edge_file, &meta.edge_weights, &meta.vertex_weights,
                                               &meta.constraint_weights, &meta.constraint_variances};
                //--------------------------------------------------------------------------------------------

                std::stringstream ss(definition);
                size_t i = 0;
                while (std::getline(ss, *p[i++], '|') && (i < p.size()))
                    ;



                // default path for all files, change this in the if you want to use another
                meta.root = common::file::utils::get_static_root();
                return meta;
            }
        };

        //! Abstraction of graph data
        struct Graph : common::HasRepr
        {
            //! Meta data
            Meta meta;

            //! Adjacency list
            std::vector<std::vector<std::pair<int, double>>> adjacency_list;

            //! Edge list
            std::vector<std::tuple<int, int, double>> edges;

            //! Edge weights
            std::vector<double> edge_weights;

            //! Vertex weights
            std::vector<double> vertex_weights;

            //! Constraint weights
            std::vector<double> constraint_weights;


            // added by saba------------------------------------------------------------------------------
            std::vector<double> constraint_variances;
            //--------------------------------------------------------------------------------------------

            double chance_cons;

            //! Flag to denoted wheter the graph has been loaded in memory
            bool loaded = false;

            //! Constructor from meta data
            Graph(const Meta &meta) : meta(meta), chance_cons(0.0) {}


            //! Constructor from string
            Graph(const std::string &definition) : Graph(Meta::from_string(definition)) {}

            //! String representation
            [[nodiscard]] std::string repr() const override
            {

                return fmt::format("<GraphInstance {} {} {} {} {}>", meta.edge_file, meta.edge_weights,
                                   meta.vertex_weights, meta.constraint_weights, meta.constraint_variances);
            }


            //! Load method
            virtual void load()
            {   
                const auto contents = common::file::as_text_vector(meta.root / meta.edge_file);
                meta.digraph = static_cast<bool>(std::stoi(contents[0]));

                if (!meta.edge_weights.empty() && meta.edge_weights != "NULL")
                    edge_weights = common::file::as_numeric_vector<double>(meta.root / meta.edge_weights);
                else
                    edge_weights = std::vector<double>(contents.size() - 1, 1.0);

                for (size_t i = 1; i < contents.size(); i++)
                {
                    const auto str = contents[i];
                    const auto space = str.find_first_of(' ');
                    auto source = std::stoi(str.substr(0, space));
                    auto target = std::stoi(str.substr(space + 1));

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
                    vertex_weights = common::file::as_numeric_vector<double>(meta.root / meta.vertex_weights);
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

                
                if (!meta.constraint_variances.empty() && meta.constraint_variances != "NULL")
                {
                    try
                    {
                        chance_cons = std::stod(meta.constraint_variances);
                        constraint_variances = std::vector<double>(meta.is_edge ? edges.size() : meta.n_vertices, 1.0);
                    }
                    catch (...)
                    {
                        chance_cons = 1.0;
                        constraint_variances =
                            ioh::common::file::as_numeric_vector<double>(meta.root / meta.constraint_variances);
                    }
                }
                else
                {
                    constraint_variances = std::vector<double>(meta.is_edge ? edges.size() : meta.n_vertices, 1.0);
                }

                IOH_DBG(xdebug, "loaded graph problem");
                IOH_DBG(xdebug, "number of edges:  " << edges.size());
                IOH_DBG(xdebug, "number of vertices: " << meta.n_vertices);
                IOH_DBG(xdebug, "number of edge_weights: " << edge_weights.size());
                IOH_DBG(xdebug, "number of vertex_weights: " << vertex_weights.size());
                IOH_DBG(xdebug, "number of constraints: " << constraint_weights.size());
                // added by saba--------------------------------------------------------------
                IOH_DBG(xdebug, "number of variances: " << constraint_variances.size());
                //---------------------------------------------------------------------------
                loaded = true;
            }

            //! Accessor for dimension, loads the graph if it is not loaded
            int dimension()
            {
                if (!loaded)
                    load();
                return meta.is_edge ? static_cast<int>(edges.size()) : meta.n_vertices;
            }
        };

    } // namespace graph

    //! Specific constraints function for submodular problems
    struct GraphConstraint : Constraint<int>
    {

        //! Shared ptr to the graph in memory
        std::shared_ptr<graph::Graph> graph;

        /**
         * @brief Construct a new Graph Constraint object
         *
         * @param graph A shared ptr to a graph
         */
        GraphConstraint(const std::shared_ptr<graph::Graph> &graph) :
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
            double count = 0.;
            for (size_t source = 0; source < x.size(); source++)
            {
                violation_ += graph->constraint_weights[source] * x[source];
                count += (double)x[source] * graph->constraint_variances[source];
            }
            violation_ += sqrt(count) * graph->chance_cons;
            return violation_ > graph->meta.constraint_limit;
        }

        //! Compute the penalty by scaling the violotation
        [[nodiscard]] double penalty() const override { return weight * pow(graph->meta.constraint_limit - violation(), exponent); }

        //! String representation
        [[nodiscard]] std::string repr() const override { return fmt::format("<GraphConstraint {}>", violation()); }
    };

    //! Implementation of the Graph Problem
    struct GraphProblem : IntegerSingleObjective
    {

        //! Shared ptr to the graph in memory
        std::shared_ptr<graph::Graph> graph;

        /**
         * @brief Construct a new Graph Problem object
         *
         * @param problem_id the id of the problem
         * @param instance the instance id
         * @param name the name of the problem
         * @param graph the graph data
         */
        GraphProblem(const int problem_id, const int instance, const std::string &name,
                     const std::shared_ptr<graph::Graph> &graph) :
            IntegerSingleObjective(
                MetaData(problem_id, instance, name, graph->dimension(), common::OptimizationType::MAX),
                Bounds<int>(graph->dimension()), ConstraintSet<int>(std::make_shared<GraphConstraint>(graph))),
            graph(graph)
        {
        }
    };

    //! Graph Problem Type
    template <typename ProblemType>
    struct GraphProblemType : GraphProblem,
                              InstanceBasedProblem,
                              AutomaticProblemRegistration<ProblemType, IntegerSingleObjective>,
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
        template <typename G = graph::Graph>
        static Constructors<ProblemType, int, int> get_constructors(const fs::path &path)
        {
            Constructors<ProblemType, int, int> constructors;

            const auto root_path = path.parent_path();

            auto graphs = common::file::as_text_vector<std::shared_ptr<G>>(path);
            int i = ProblemType::default_id;
            for (auto &graph : graphs)
            {
                graph->meta.root = root_path;
                constructors.push_back({[graph, i](int, int) { return ProblemType(i, 1, graph); }, i++, std::nullopt});
            }
            return constructors;
        }

        //! Helper to load problems from a file
        //! Use if you want to manually load more graph based files
        template <typename... Args>
        static void load_graph_instances(const std::optional<fs::path> &path = std::nullopt)
        {
            for (auto &ci : InstanceBasedProblem::load_instances<ProblemType, int, int>(path))
            {
                
                const auto name = fmt::format("{}{}", ioh::common::class_name<ProblemType>(), std::get<1>(ci));
                auto c = [c = std::get<0>(ci)](Args &&...params) {
                    return std::make_unique<ProblemType>(c(std::forward<Args>(params)...));
                };
                ioh::common::Factory<IntegerSingleObjective, Args...>::instance().include(name, std::get<1>(ci), c);
                ioh::common::Factory<submodular::GraphProblem, Args...>::instance().include(name, std::get<1>(ci), c);
            }
        }
    };
} // namespace ioh::problem::submodular
