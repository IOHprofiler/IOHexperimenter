#pragma once

#include "ioh/common.hpp"
#include "ioh/problem/problem.hpp"


namespace ioh::problem::submodular
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
                while (std::getline(ss, *p[i++], '|') && (i < p.size()))
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

        struct Graph : common::HasRepr
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

            [[nodiscard]] std::string repr() const override
            {
                return fmt::format("<GraphInstance {} {} {} {}>", meta.edge_file, meta.edge_weights,
                                   meta.vertex_weights, meta.constraint_weights);
            }

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

    struct GraphConstraint : Constraint<int>
    {
        std::shared_ptr<graph::Graph> graph;

        GraphConstraint(const std::shared_ptr<graph::Graph> &graph) : 
            Constraint(constraint::Enforced::HARD), graph(graph) {}

        bool compute_violation(const std::vector<int>& x) override { 
            
            violation_ = 0;
             
            int count = 0;
            for (size_t source = 0; source < x.size(); source++)
            {
                violation_ += graph->constraint_weights[source] * x[source];
                count += x[source];
            }

            violation_ += sqrt(count) * graph->meta.chance_cons;
            return violation_ > graph->meta.constraint_limit;
        }

        double penalty() const override {
            return weight * pow(graph->meta.constraint_limit - violation(), exponent);
        }
        

        std::string repr() const override { return fmt::format("<GraphConstraint {}>", violation()); }
    };


    struct GraphProblem : Integer
    {
        std::shared_ptr<graph::Graph> graph;

        GraphProblem(const int problem_id, const int instance, const std::string &name,
                     const std::shared_ptr<graph::Graph> &graph) :
            Integer(MetaData(problem_id, instance, name, graph->dimension(), common::OptimizationType::MAX),
                    Bounds<int>(graph->dimension()),
                    ConstraintSet<int>(std::make_shared<GraphConstraint>(graph))    
                ),
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
                constructors.push_back({[graph, i](int, int) { return ProblemType(i, 1, graph); }, i++});
            }
            return constructors;
        }
    
        //! Helper to load problems from a file
        template <typename... Args>
        static void load_graph_instances(const fs::path &path)
        {
            for (auto &ci : InstanceBasedProblem::load_instances<ProblemType, int, int>(path))
            {
                const auto name = fmt::format("{}{}", ioh::common::class_name<ProblemType>(), ci.second);
                auto c = [c = ci.first](Args &&...params) {
                    return std::make_unique<ProblemType>(c(std::forward<Args>(params)...));
                };
                ioh::common::Factory<Integer, Args...>::instance().include(name, ci.second, c);
                ioh::common::Factory<submodular::GraphProblem, Args...>::instance().include(name, ci.second, c);
            }
        }       
    };
} // namespace ioh::problem::submodular
