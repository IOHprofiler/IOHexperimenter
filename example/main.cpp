#include "ioh.hpp"


void show_registered_objects()
{
    {
        const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::Real>::instance();
        const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Real>::instance();

        std::cout << "Registered Real Problems:\n";

        for (auto &[id, name] : problem_factory.map())
            std::cout << id << ", " << name << std::endl;

        std::cout << "\nRegistered Real Suites:\n";
        for (auto &[id, name] : suite_factory.map())
            std::cout << id << ", " << name << std::endl;
    }
    {
        const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::Integer>::instance();
        const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Integer>::instance();

        std::cout << "\nRegistered Integer Problems:\n";

        for (auto &[id, name] : problem_factory.map())
            std::cout << id << ", " << name << std::endl;

        std::cout << "\nRegistered Integer Suites:\n";
        for (auto &[id, name] : suite_factory.map())
            std::cout << id << ", " << name << std::endl;
    }
}

template <typename Iterator>
void print(const Iterator x, const std::string &del = " ")
{
    for (auto e : x)
    {
        std::cout << e << del;
    }
    std::cout << std::endl;
}


namespace ioh::problem
{
    struct GraphMeta
    {
        std::string edge_file;
        std::string edge_weights;
        std::string vertex_weights;
        std::string constraint_weights;
        fs::path root;
        bool is_edge = false;
        double chance_cons = 0.0;

        static GraphMeta from_string(const std::string &definition)
        {
            GraphMeta meta;
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
        GraphMeta meta;
        std::vector<std::vector<double>> adjacency_matrix;
        std::vector<std::tuple<int, int, double>> edges;
        std::vector<double> edge_weights;
        std::vector<double> vertex_weights;
        std::vector<double> constraint_weights;
        bool digraph = false;
        double constraint_limit = std::numeric_limits<double>::infinity();
        int n_vertices = 0;
        bool loaded = false;

        Graph(const GraphMeta &meta) : meta(meta) {}

        Graph(const std::string &definition) : Graph(GraphMeta::from_string(definition)) {}

        std::string repr() const
        {
            return fmt::format("<GraphInstance {} {} {} {}>", meta.edge_file, meta.edge_weights, meta.vertex_weights,
                               meta.constraint_weights);
        }

        void load()
        {
            using Matrix = std::vector<std::vector<double>>;

            const auto contents = ioh::common::file::as_text_vector(meta.root / meta.edge_file);
            digraph = static_cast<bool>(std::stoi(contents.at(0)));


            if (!meta.edge_weights.empty() && meta.edge_weights != "NULL")
                edge_weights = ioh::common::file::as_numeric_vector<double>(meta.root / meta.edge_weights);
            else
                edge_weights = std::vector<double>(contents.size() - 1, 1.0);

            for (size_t i = 1; i < contents.size(); i++)
            {
                int source, target;
                std::stringstream{contents.at(i)} >> source >> target;
                n_vertices = std::max(n_vertices, std::max(source, target));
                edges.push_back(std::make_tuple(source - 1, target - 1, edge_weights.at(i - 1)));
            }

            adjacency_matrix = Matrix(n_vertices, std::vector<double>(n_vertices));

            for (const auto &[source, target, weight] : edges)
            {
                adjacency_matrix.at(source).at(target) = weight;
                if(!digraph)
                    adjacency_matrix.at(target).at(source) = weight;
            }

            if (!meta.vertex_weights.empty() && meta.vertex_weights != "NULL")
                vertex_weights = ioh::common::file::as_numeric_vector<double>(meta.root / meta.vertex_weights);
            else
                vertex_weights = std::vector<double>(n_vertices, 1.);

            if (!meta.constraint_weights.empty() && meta.constraint_weights != "NULL")
            {
                constraint_weights = ioh::common::file::as_numeric_vector<double>(meta.root / meta.constraint_weights);
                constraint_limit = constraint_weights.back();
                constraint_weights.pop_back();
            }
            else
            {
                constraint_weights = std::vector<double>(meta.is_edge ? edges.size() : n_vertices, 1);
            }

            IOH_DBG(warning, "loaded graph problem");
            IOH_DBG(warning, "number of edges:  " << edges.size());
            IOH_DBG(warning, "number of vertices: " << n_vertices);
            IOH_DBG(warning, "number of edge_weights: " << edge_weights.size());
            IOH_DBG(warning, "number of vertex_weights: " << vertex_weights.size());
            IOH_DBG(warning, "number of constraints: " << constraint_weights.size());

            loaded = true;
        }

        int dimension()
        {
            if (!loaded)
                load();
            return meta.is_edge ? edges.size() : n_vertices;
        }
    };

    struct GraphProblem_ : Integer
    {
        Graph graph;

        GraphProblem_(const int problem_id, const int instance, const std::string &name, Graph graph) :
            Integer(MetaData(problem_id, instance, name, graph.dimension(), common::OptimizationType::Maximization),
                    Constraint<int>(graph.dimension(), 0, 1)),
            graph(graph)
        {
        }
    };

    template <typename ProblemType>
    class GGproblem : public GraphProblem_, InstanceBasedProblem, AutomaticProblemRegistration<ProblemType, Integer>
    {
    public:
        using GraphProblem_::GraphProblem_;

        /**
         * @brief Get the constructors used in explicit specialization of load_isntances method if InstanceBasedProblem
         *
         * @param path
         * @return InstanceBasedProblem::Constructors<ProblemType, int, int>
         */
        static InstanceBasedProblem::Constructors<ProblemType, int, int> get_constructors(const fs::path &path)
        {
            InstanceBasedProblem::Constructors<ProblemType, int, int> constructors;

            auto root_path = path.parent_path();
            auto graphs = common::file::as_text_vector<Graph>(path);
            int i = ProblemType::default_id;
            for (auto graph : graphs)
            {
                graph.meta.root = root_path;
                constructors.push_back({[graph, i](int, int) { return ProblemType(i, 1, graph); }, i++});
            }
            return constructors;
        }
    };


    struct MaxCutt : GGproblem<MaxCutt>
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
        MaxCutt(const int problem_id, const int, Graph graph) :
            GGproblem(problem_id, 1, fmt::format("MaxCutt{}", problem_id), graph)
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
                const bool selected = x.at(source);
                const auto edges = graph.adjacency_matrix.at(source);

                constraint += graph.constraint_weights.at(source) * selected;
                count += selected;

                for (size_t target = 0; target < edges.size(); target++)
                {
                    const double weight = edges.at(target);
                    const bool has_edge = weight != 0;
                    const bool unselected_node_at_egde = selected && has_edge && x.at(target) == 0;
                    const bool selected_node_wo_egde = !selected && has_edge && x.at(target) == 1 && graph.digraph;

                    result += weight * unselected_node_at_egde;
                    result -= weight * selected_node_wo_egde;
                }
            }
            constraint += sqrt(count) * graph.meta.chance_cons;
            if (constraint > graph.constraint_limit)
                result = graph.constraint_limit - constraint;
            return result;
        }
    };

    template <>
    InstanceBasedProblem::Constructors<MaxCutt, int, int>
    InstanceBasedProblem::load_instances<MaxCutt>(const std::optional<fs::path> &definitions_file)
    {
        return GGproblem<MaxCutt>::get_constructors(
            definitions_file.value_or(common::file::utils::find_static_file("example_list_maxcut")));
    }

    struct MaxCoveragee : GGproblem<MaxCoveragee>
    {
        static inline int default_id = 2100;
        std::vector<bool> is_covered;
        /**
         * @brief Construct a new MaxCoverage object. Suggested usage is via the factory.
         * If you want to create your own objects, please be sure to pass a correct graph instance.
         *
         * @param problem_id the id to the problem
         * @param instance_id for instance based problems this is ignored
         * @param graph the graph object on which to operate
         */
        MaxCoveragee(const int problem_id, const int, Graph graph) :
            GGproblem(problem_id, 1, fmt::format("MaxCoveragee{}", problem_id), graph),
            is_covered(std::vector<bool>(graph.dimension(), false))
        {
            objective_.x = std::vector<int>(graph.dimension(), 1);
            objective_.y = evaluate(objective_.x);
        }

        double evaluate(const std::vector<int> &x) override
        {
            std::fill(is_covered.begin(), is_covered.end(), false);

            double result = 0, constraint = 0;
            int count = 0;
            for (size_t source = 0; source < x.size(); source++)
            {
                if (x.at(source))
                {
                    constraint += graph.constraint_weights.at(source);
                    count++;

                    if (!is_covered[source])
                    {
                        result += graph.vertex_weights.at(source);
                        is_covered[source] = true;
                    }

                    const auto edges = graph.adjacency_matrix.at(source);
                    for (size_t target = 0; target < edges.size(); target++)
                    {
                        if (!is_covered[target] && edges.at(target) != 0)
                        {
                            result += graph.vertex_weights.at(target);
                            is_covered[source] = true;
                        }
                    }
                }
            }
            constraint += sqrt(count) * graph.meta.chance_cons;
            if (constraint > graph.constraint_limit)
                result = graph.constraint_limit - constraint;
            return result;
        }
    };

    template <>
    InstanceBasedProblem::Constructors<MaxCoveragee, int, int>
    InstanceBasedProblem::load_instances<MaxCoveragee>(const std::optional<fs::path> &definitions_file)
    {
        return GGproblem<MaxCoveragee>::get_constructors(
            definitions_file.value_or(common::file::utils::find_static_file("example_list_maxcoverage")));
    }

    struct MaxInfluencee : GGproblem<MaxInfluencee>
    {
        static inline int default_id = 2200;
        int simulation_reps = 100;

        std::vector<bool> is_activated;
        /**
         * @brief Construct a new MaxInfluence object. Suggested usage is via the factory.
         * If you want to create your own objects, please be sure to pass a correct graph instance.
         *
         * @param problem_id the id to the problem
         * @param instance_id for instance based problems this is ignored
         * @param graph the graph object on which to operate
         */
        MaxInfluencee(const int problem_id, const int, Graph graph) :
            GGproblem(problem_id, 1, fmt::format("MaxInfluencee{}", problem_id), graph),
            is_activated(std::vector<bool>(graph.dimension(), false))
        {
            objective_.x = std::vector<int>(graph.dimension(), 1);
            // objective_.y = evaluate(objective_.x);
            std::cout << graph.meta.edge_file << std::endl;
        }

        double random_spread_count(const std::vector<int> &x)
        {
            using namespace ioh::common::random;
            double total = 0;

            std::queue<int> visits;

            for(size_t i=0; i < x.size();i++){
                is_activated[i] = static_cast<bool>(x.at(i));
                if(x.at(i) != 0)
                    visits.push(i);
            }


            while (!visits.empty()) // Terminate when no more spreading
            {
                int source = visits.front();
                visits.pop();

                const auto edges = graph.adjacency_matrix.at(source);
                for (size_t target = 0; target < edges.size(); target++)
                {
                    if(edges.at(target) != 0) {
                        const double r = 0.5; //ioh::common::random::real();
                        if (!is_activated[target] && r <= edges.at(target)){
                            is_activated[target] = true;
                            visits.push(target);
                            total += graph.vertex_weights.at(target);
                        }
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
                constraint += graph.constraint_weights.at(source) * x.at(source);
                result += graph.vertex_weights.at(source) * x.at(source); 
                count += x.at(source);
            }

            constraint += sqrt(count) * graph.meta.chance_cons;
            if (constraint > graph.constraint_limit)
                return graph.constraint_limit - constraint;

            for(auto i=0; i < simulation_reps; i++)
                result += random_spread_count(x) / simulation_reps;
            return result;
        }
    };

    template <>
    InstanceBasedProblem::Constructors<MaxInfluencee, int, int>
    InstanceBasedProblem::load_instances<MaxInfluencee>(const std::optional<fs::path> &definitions_file)
    {
        return GGproblem<MaxInfluencee>::get_constructors(
            definitions_file.value_or(common::file::utils::find_static_file("example_list_maxinfluence")));
    }


} // namespace ioh::problem

template <typename GraphProblem>
std::shared_ptr<GraphProblem> get_graph_problem(const int instance)
{
    const fs::path path = fs::current_path();
    fs::current_path(ioh::common::file::utils::get_static_root());
    auto problem = std::make_shared<GraphProblem>(instance);
    fs::current_path(path);
    return problem;
}

template <typename P = ioh::problem::Integer>
void display(const std::shared_ptr<P> &problem)
{
    std::cout << (*problem).meta_data() << std::endl;
    // std::cout << (*problem).state().current_best.y << std::endl;
    // std::cout << (*problem)(problem->state().current_best.x) << std::endl;
    // std::cout << (*problem).state().current_best.y << std::endl;
}


int main()
{
    using namespace ioh::common;
    using namespace ioh::problem;


    const auto &problem_factory = ioh::problem::ProblemRegistry<Integer>::instance();

    auto problem = problem_factory.create("MaxInfluencee2200", 1, 1);
    // display(problem);
    auto gp = get_graph_problem<ioh::problem::submodular::MaxInfluence>(1);
    // display(gp);

    ioh::common::random::seed(10);
    auto x0 = ioh::common::random::integers(gp->meta_data().n_variables, 0, 1);

    (*gp)(x0);
    std::cout << "gp:" << gp->state().current.y << std::endl;

    (*problem)(x0);
    std::cout << "new:" << problem->state().current.y << std::endl;
}