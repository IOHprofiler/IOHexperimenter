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
        

            //! The directory of the files: that of the definitions file for a graph loaded from one, empty for
            //! the built-in graphs, whose files are in the data directory (see ioh::data::root)
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

            //! Held while the graph is loaded, so that it is loaded once, also when problems are created in parallel
            std::mutex load_mutex;

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
            /**
             * @brief The path of one of the graph's files: in the data directory for a built-in graph, otherwise in
             * the directory of its definitions file. Throws ioh::data::data_error when it does not exist.
             */
            [[nodiscard]] fs::path path(const std::string &file) const
            {
                if (meta.root.empty())
                    return ioh::data::file(file);
                auto path = meta.root / file;
                if (!fs::exists(path))
                    throw ioh::data::data_error(fmt::format("The graph file {} does not exist", path.u8string()));
                return path;
            }

            virtual void load()
            {
                // from scratch: a load that stopped (e.g. at a missing file) can be repeated
                edges.clear();
                adjacency_list.clear();
                meta.n_vertices = 0;

                const auto contents = common::file::as_text_vector(path(meta.edge_file));
                meta.digraph = static_cast<bool>(std::stoi(contents[0]));

                if (!meta.edge_weights.empty() && meta.edge_weights != "NULL")
                    edge_weights = common::file::as_numeric_vector<double>(path(meta.edge_weights));
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
                    vertex_weights = common::file::as_numeric_vector<double>(path(meta.vertex_weights));
                else
                    vertex_weights = std::vector<double>(meta.n_vertices, 1.);

                if (!meta.constraint_weights.empty() && meta.constraint_weights != "NULL")
                {
                    constraint_weights =
                        ioh::common::file::as_numeric_vector<double>(path(meta.constraint_weights));
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
                            ioh::common::file::as_numeric_vector<double>(path(meta.constraint_variances));
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
                {
                    const std::lock_guard<std::mutex> lock(load_mutex);
                    if (!loaded)
                        load();
                }
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
                Bounds<int>(graph->dimension(),0,1), ConstraintSet<int>(std::make_shared<GraphConstraint>(graph))),
            graph(graph)
        {
            // the dimension is the size of the graph, and graph problems have a single instance
            meta_data_.fixed_dimension = meta_data_.fixed_instance = true;
        }
    };

    //! Held while graphs are registered, by every graph class: they share the factories and the next free id
    inline std::mutex &graph_registration_mutex()
    {
        static std::mutex mutex;
        return mutex;
    }

    //! Base of the graph problems, which are defined by the graphs in a definitions file
    template <typename ProblemType>
    struct GraphProblemType : GraphProblem
    {
        using GraphProblem::GraphProblem;

        /**
         * @brief The registry entries of the graphs in a definitions file, one per graph: ids from
         * ProblemType::default_id on, names like MaxCut2000; the dimension and the instance are fixed
         *
         * @param path the definitions file
         * @param name the prefix of the names
         */
        template <typename G = graph::Graph>
        static std::vector<RegistryEntry<ProblemType>> graph_entries(const fs::path &path, const std::string &name)
        {
            std::vector<RegistryEntry<ProblemType>> entries;

            const auto root_path = fs::absolute(path).parent_path(); // never empty, which marks the built-in graphs

            auto graphs = common::file::as_text_vector<std::shared_ptr<G>>(path);
            int i = ProblemType::default_id;
            for (auto &graph : graphs)
            {
                graph->meta.root = root_path;
                // compute the id first: with {[graph, i]..., i++} MSVC captured the incremented i,
                // so every graph problem's meta_data id was its registry id + 1 on Windows
                const int id = i++;
                entries.push_back({id, fmt::format("{}{}", name, id),
                                   [graph, id](int, int) { return std::make_shared<ProblemType>(id, 1, graph); }});
            }
            return entries;
        }

        /**
         * @brief The registry entries of the built-in graphs of a class, from the manifest compiled into the library
         * (ioh::data::manifest_entries): a graph's data are loaded when its first problem is created
         *
         * @param problem_class the name of the class (e.g. MaxCut)
         */
        template <typename G = graph::Graph>
        static std::vector<RegistryEntry<ProblemType>> builtin_entries(const std::string &problem_class)
        {
            std::vector<RegistryEntry<ProblemType>> entries;
            for (const auto &entry : ioh::data::manifest_entries("GraphProblem"))
                if (entry.problem_class == problem_class)
                {
                    const auto graph = std::make_shared<G>(entry.definition);
                    const int id = entry.id;
                    entries.push_back(
                        {id, entry.name, [graph, id](int, int) { return std::make_shared<ProblemType>(id, 1, graph); }});
                }
            return entries;
        }

        /**
         * @brief Registers graphs in the factories of GraphProblem and IntegerSingleObjective, and returns their ids
         * and names
         *
         * With a path, the graphs of that definitions file are added after the existing ones: their ids continue
         * after the highest id in the GraphProblem factory, in file order, their names are the class name and the
         * id (e.g. MaxCut2309), and their problems report both. The file is read before anything is registered,
         * and loading the same file again registers nothing and returns the same ids. Without a path, the class's
         * own registry_entries() are registered under their ids (nothing to do for the built-in classes). An id
         * that is taken throws std::invalid_argument (see register_problem).
         *
         * @param path a definitions file
         */
        template <typename... Args>
        static std::map<int, std::string> load_graph_instances(const std::optional<fs::path> &path = std::nullopt)
        {
            auto &graphs = common::Factory<GraphProblem, Args...>::instance();
            auto &integers = common::Factory<IntegerSingleObjective, Args...>::instance();
            const auto include = [&](const RegistryEntry<ProblemType> &entry, const int id, const std::string &name) {
                auto create = [create = entry.create, id, name](Args &&...params) {
                    auto problem = create(std::forward<Args>(params)...);
                    if (problem->meta_data().problem_id != id)
                    {
                        problem->set_id(id);
                        problem->set_name(name);
                    }
                    return problem;
                };
                // the family's factory first: it throws for a taken id, before anything is registered elsewhere
                graphs.include(name, problem::detail::registered_id<GraphProblem>(graphs, id, name), create, true);
                integers.include(name, problem::detail::registered_id<IntegerSingleObjective>(integers, id, name),
                                 create);
            };

            // the files loaded before, with the ids and names of their graphs
            static std::map<std::string, std::map<int, std::string>> loaded;
            const std::lock_guard<std::mutex> lock(graph_registration_mutex());

            std::map<int, std::string> result;
            if (!path)
            {
                std::vector<RegistryEntry<ProblemType>> missing;
                const auto entries = ProblemType::registry_entries();
                problem::detail::check_unique(entries);
                for (const auto &entry : entries)
                {
                    result[entry.id] = entry.name;
                    const auto map = graphs.map();
                    if (const auto it = map.find(entry.id); it != map.end() && it->second == entry.name)
                        continue;
                    (void)problem::detail::registered_id<GraphProblem>(graphs, entry.id, entry.name);
                    (void)problem::detail::registered_id<IntegerSingleObjective>(integers, entry.id, entry.name);
                    missing.push_back(entry);
                }
                for (const auto &entry : missing)
                    include(entry, entry.id, entry.name);
                return result;
            }

            if (!fs::is_regular_file(*path))
                throw std::invalid_argument(fmt::format("Cannot read the graph definitions file {}", path->u8string()));
            const auto key = fs::weakly_canonical(*path).u8string();
            if (const auto it = loaded.find(key); it != loaded.end())
                return it->second;

            const auto entries = ProblemType::registry_entries(path);
            const int first_id = graphs.ids().back() + 1;
            std::vector<std::string> names;
            for (const auto &entry : entries)
            {
                const int id = first_id + static_cast<int>(names.size());
                // the names of graph_entries end with the id from the file, which is replaced
                const auto old_id = std::to_string(entry.id);
                const auto ends_with_id = entry.name.size() > old_id.size() &&
                    entry.name.compare(entry.name.size() - old_id.size(), old_id.size(), old_id) == 0;
                names.push_back(ends_with_id
                                    ? entry.name.substr(0, entry.name.size() - old_id.size()) + std::to_string(id)
                                    : entry.name);
                (void)problem::detail::registered_id<GraphProblem>(graphs, id, names.back());
                (void)problem::detail::registered_id<IntegerSingleObjective>(integers, id, names.back());
                result[id] = names.back();
            }
            if (result.size() != std::set<std::string>(names.begin(), names.end()).size())
                throw std::invalid_argument(fmt::format("The graph definitions file {} gives two graphs the same name",
                                                        path->u8string()));
            for (size_t i = 0; i < entries.size(); ++i)
                include(entries[i], first_id + static_cast<int>(i), names[i]);
            loaded[key] = result;
            return result;
        }
    };
} // namespace ioh::problem::submodular
