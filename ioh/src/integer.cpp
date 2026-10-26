#include "common.hpp"

#include "registry/families.hpp"

namespace ioh_python::integer
{
    //! The docstring of each PBO class (every class of ioh::registry::pbo_classes needs one)
    template <class T>
    constexpr const char *pbo_doc = nullptr;

    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::OneMax> = R"pbdoc(
            OneMax:
            {0,1}^n → [0..n], x ↦ ∑_{i=1}^n x_i.

        )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::LeadingOnes> = R"pbdoc(
            LeadingOnes:
            {0,1}^n → [0..n], x ↦ max{i∈[0..n] ∣ ∀j≤i: x_j=1}

        )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::Linear> = R"pbdoc(
            A Linear Function with Harmonic Weights:
            {0,1}^n → ℝ, x ↦ ∑_i i * x_i
        )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::OneMaxDummy1> = R"pbdoc(
                A variant of OneMax applying the Dummy transformation of W-model. m = 0.5n.
                Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
            )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::OneMaxDummy2> = R"pbdoc(
                A variant of OneMax applying the Dummy transformation of W-model. m = 0.9n.
                Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
            )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::OneMaxNeutrality> = R"pbdoc(
                A variant of OneMax applying the Neutrality transformation of W-model. \mu = 3.
                Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::OneMaxEpistasis> = R"pbdoc(
                A variant of OneMax applying the Epistasis transformation of W-model. \nu = 4.
                Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::OneMaxRuggedness1> = R"pbdoc(
                A variant of OneMax applying the first Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::OneMaxRuggedness2> = R"pbdoc(
                A variant of OneMax applying the second Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::OneMaxRuggedness3> = R"pbdoc(
                A variant of OneMax applying the third Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::LeadingOnesDummy1> = R"pbdoc(
                A variant of LeadingOnes applying the Dummy transformation of W-model. m = 0.5n.
                Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
            )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::LeadingOnesDummy2> = R"pbdoc(
                A variant of LeadingOnes applying the Dummy transformation of W-model. m = 0.9n.
                Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
            )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::LeadingOnesNeutrality> = R"pbdoc(
                A variant of LeadingOnes applying the Neutrality transformation of W-model. \mu = 3.
                Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::LeadingOnesEpistasis> = R"pbdoc(
                A variant of LeadingOnes applying the Epistasis transformation of W-model. \nu = 4.
                Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::LeadingOnesRuggedness1> = R"pbdoc(
                A variant of LeadingOnes applying the first Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::LeadingOnesRuggedness2> = R"pbdoc(
                A variant of LeadingOnes applying the second Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::LeadingOnesRuggedness3> = R"pbdoc(
                A variant of LeadingOnes applying the third Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::LABS> = R"pbdoc(
            Low Autocorrelation Binary Sequences (LABS):
            x ↦ n^2 / 2∑_{k=1}^{n-1}(∑_{i=1}^{n−k}s_is_{i+k})^2, where s_i = 2x_i − 1

        )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::IsingRing> = R"pbdoc(
            The Ising Spin Glass model arose in solid-state physics and statistical mechanics, aiming to 
            describe simple interactions within many-particle systems. A compact form used here is 
            x ↦ ∑_{{u,v} \in E}[x_ux_v - (1-x_u) (1-x_v)]. E is defined over a one-dimensional lattice.
            Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.

        )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::IsingTorus> = R"pbdoc(
            The Ising Spin Glass model arose in solid-state physics and statistical mechanics, aiming to 
            describe simple interactions within many-particle systems. A compact form used here is 
            x ↦ ∑_{{u,v} \in E}[x_ux_v - (1-x_u) (1-x_v)]. E is defined over a two-dimensional lattice.
            Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.
        )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::IsingTriangular> = R"pbdoc(
            The Ising Spin Glass model arose in solid-state physics and statistical mechanics, aiming to 
            describe simple interactions within many-particle systems. A compact form used here is 
            x ↦ ∑_{{u,v} \in E}[x_ux_v - (1-x_u) (1-x_v)]. E is defined over a three-dimensional lattice.
            Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.
        )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::MIS> = R"pbdoc(
            The maximum independent vertex set (MIVS) formulated as 
            x ↦ ∑_i x_i - n ∑_{i,j} x_i x_j e_{i,j}, where e_{i,j} = 1 if {i,j} \in E, otherwise e_{i,j} = 0.
            Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.                       
        )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::NQueens> = R"pbdoc(
            The N-queens problem (NQP) is defined as the task to place N queens on an N*N chessboard in such a 
            way that they cannot attack each other.   
            Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.                      
        )pbdoc";
    template <>
    constexpr const char *pbo_doc<ioh::problem::pbo::ConcatenatedTrap> = R"pbdoc(
        Concatenated Trap (CT) is defined by partitioning a bit-string into segments of length k and 
        concatenating m = n/k trap functions that takes each segment as input. The trap function is 
        defined as follows: f_trap(u) = 1 if the k number u of ones satisfies u = k and 
        f_trap(u) = (k−1−u)/k otherwise. We use k = 5 in our experiments.
        Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.
        )pbdoc";
    //! Defines the PBO class T, named by its registry name, which Python cannot subclass. A class without a docstring
    //! has no Python class of its own (NKLandscapes, as on master: its problems are PBO objects, and the golden
    //! registry views list one view per class)
    template <class T>
    void define_pbo_class(nb::module_ &m)
    {
        if constexpr (pbo_doc<T> != nullptr)
            nb::class_<T, ioh::problem::PBO>(m, T::registry_name, nb::is_final(), pbo_doc<T>)
                .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));
    }

    //! Defines the PBO classes of a type list
    template <class... Ts>
    void define_pbo_classes(nb::module_ &m, ioh::registry::type_list<Ts...>)
    {
        (define_pbo_class<Ts>(m), ...);
    }

    //! A W-model that Python can subclass, overriding wmodel_evaluate
    class WModelTrampoline : public ioh::problem::WModel
    {
    public:
        NB_TRAMPOLINE(ioh::problem::WModel);

    protected:
        int wmodel_evaluate(const std::vector<int> &x) override { NB_OVERRIDE_PURE(wmodel_evaluate, x); }
    };

    //! Makes the protected wmodel_evaluate accessible for the binding
    class WModelPublisher : public ioh::problem::WModel
    {
    public:
        using ioh::problem::WModel::wmodel_evaluate;
    };

    //! Adds the graphs of a definitions file to the graph problems of ProblemType (load_instances)
    template <typename ProblemType>
    std::map<int, std::string> load_graph_instances(const std::optional<fs::path> &path)
    {
        if (!path &&
            PyErr_WarnEx(PyExc_DeprecationWarning,
                         "load_instances() without a path does nothing: the built-in graphs are always registered",
                         1) < 0)
            throw nb::python_error();
        return ioh::problem::submodular::GraphProblemType<ProblemType>::template load_graph_instances<int, int>(path);
    }

    constexpr auto load_instances_doc = R"pbdoc(
    Adds the graphs of a definitions file after the existing graph problems, in GraphProblem and
    IntegerSingleObjective, and returns their {id: name}. Their ids continue after the highest graph problem id, and
    their names are the class name and the id. Loading the same file again returns the same ids. Calling it without
    a path is deprecated and does nothing.
)pbdoc";
} // namespace ioh_python::integer


void define_pbo_problems(nb::module_ &m)
{
    using ioh::problem::PBO;

    nb::class_<PBO, ioh::problem::IntegerSingleObjective>(m, "PBO",
                                                          R"pbdoc(
            Pseudo-Boolean Optimization (PBO) problem set.
            
            Contains 25 test functions taking their domain on {0, 1}^n, where n is the length of bitstrings.

            In PBO, we cover some theory-motivated function, e.g., OneMax and LeadingOnes
            as well as others with more practical relevance, e.g., the NK Landscape [DoerrYHWSB20].
            We also utilized the so-called W-model for generating/enriching the problem set [WeiseW18].

            Reference
            ---------
            [DoerrYHWSB20] Carola Doerr, Furong Ye, Naama Horesh, Hao Wang, Ofer M. Shir, and Thomas Bäck.
            "Benchmarking discrete optimization heuristics with IOHprofiler." Applied Soft Computing 88 (2020): 106027.

            [WeiseW18] Thomas Weise and Zijun Wu. "Difficult features of combinatorial optimization problems and
            the tunable w-model benchmark problem for simulating them." In Proceedings of the Genetic
            and Evolutionary Computation Conference Companion, pp. 1769-1776. 2018.

        )pbdoc")
        .def_static(
            "create",
            [](const std::string &name, int iid, int dim) {
                return ioh::common::Factory<PBO, int, int>::instance().create(name, iid, dim);
            },
            nb::arg("problem_name"), nb::arg("instance_id"), nb::arg("dimension"),
            R"pbdoc(
                Create a problem instance

                Parameters
                ----------
                    problem_name: str
                        a string indicating the problem name. 
                    instance_id: int
                        an integer identifier of the problem instance
                    dimension: int
                        the dimensionality of the search space
            )pbdoc")
        .def_static(
            "create",
            [](int id, int iid, int dim) { return ioh::common::Factory<PBO, int, int>::instance().create(id, iid, dim); },
            nb::arg("problem_id"), nb::arg("instance_id"), nb::arg("dimension"),
            R"pbdoc(
                Create a problem instance

                Parameters
                ----------
                    problem_name: int
                        a string indicating the problem name. 
                    instance_id: int
                        an integer identifier of the problem instance
                    dimension: int
                        the dimensionality of the search space
            )pbdoc")
        .def_prop_ro_static(
            "problems", [](nb::handle) { return ioh::common::Factory<PBO, int, int>::instance().map(); },
            "All registered problems");

    ioh_python::integer::define_pbo_classes(m, ioh::registry::pbo_classes{});
}


void define_wmodels(nb::module_ &m)
{
    using ioh::problem::WModel;
    using ioh::problem::wmodel::WModelLeadingOnes;
    using ioh::problem::wmodel::WModelOneMax;
    using ioh_python::integer::WModelPublisher;

    nb::class_<WModel, ioh::problem::IntegerSingleObjective, ioh_python::integer::WModelTrampoline>(
        m, "AbstractWModel",
        R"pbdoc(
            An abstract W-model class. Please apply the WModelOneMax and WModelLeadingOnes classes.
             
            W-model problems applies four basic transformations: reduction of dummy variables,
            neutrality, epistasis, and fitness perturbation on benchmark problems. Each transformation
            is configured by the corresponding argument dummy_select_rate, epistasis_block_size,
            neutrality_mu, and ruggedness_gamma.

            The details of the transformations can be found in
            https://www.sciencedirect.com/science/article/pii/S1568494619308099

            We have acknowledged Thomas Weise, proposing the original W-model
            https://dl.acm.org/doi/abs/10.1145/3205651.3208240 and having discussions supporting this work.

            Reference
            ---------
            [WeiseW18] Thomas Weise and Zijun Wu. "Difficult features of combinatorial optimization problems
            and the tunable w-model benchmark problem for simulating them." Proc. of the Genetic and 
            Evolutionary Computation Conference Companion, 1769-1776, 2018

            [DoerrYHWSB20] Carola Doerr, Furong Ye, Naama Horesh, Hao Wang, Ofer M. Shir, Thomas Bäck.
            "Benchmarking discrete optimization heuristics with IOHprofiler."
            Applied Soft Computing, 88: 106027, 2020 

        )pbdoc")
        .def(nb::init<int, int, int, std::string, double, int, int, int>(), nb::arg("problem_id"), nb::arg("instance"),
             nb::arg("n_variables"), nb::arg("name"), nb::arg("dummy_select_rate") = 0.0,
             nb::arg("epistasis_block_size") = 0, nb::arg("neutrality_mu") = 0, nb::arg("ruggedness_gamma") = 0)
        .def("wmodel_evaluate",
             [](WModel &self, const ioh_python::IntArray1D &x) {
                 return (self.*(&WModelPublisher::wmodel_evaluate))(ioh_python::to_vector<int>(x));
             },
             ioh_python::array_arg<int>())
        .def("wmodel_evaluate", &WModelPublisher::wmodel_evaluate);

    nb::class_<WModelLeadingOnes, WModel>(m, "WModelLeadingOnes",
                                          R"pbdoc(
            A W-model problem built on LeadingOnes.
             
            This W-model problem applies four basic transformations: reduction of dummy variables,
            neutrality, epistasis, and fitness perturbation on the LeadingOnes problem. Each transformation
            is configured by the corresponding argument dummy_select_rate, epistasis_block_size,
            neutrality_mu, and ruggedness_gamma.

            The definitions of LeadingOnes and the transformations can be found in
            https://www.sciencedirect.com/science/article/pii/S1568494619308099

            We have acknowledged Thomas Weise, proposing the original W-model
            https://dl.acm.org/doi/abs/10.1145/3205651.3208240 and having discussions supporting this work.

            Reference
            ---------
            [WeiseW18] Thomas Weise and Zijun Wu. "Difficult features of combinatorial optimization problems
            and the tunable w-model benchmark problem for simulating them." Proc. of the Genetic and 
            Evolutionary Computation Conference Companion, 1769-1776, 2018

            [DoerrYHWSB20] Carola Doerr, Furong Ye, Naama Horesh, Hao Wang, Ofer M. Shir, Thomas Bäck.
            "Benchmarking discrete optimization heuristics with IOHprofiler."
            Applied Soft Computing, 88: 106027, 2020 

        )pbdoc")
        .def(nb::init<int, int, double, int, int, int>(), nb::arg("instance"), nb::arg("n_variables"),
             nb::arg("dummy_select_rate") = 0.0, nb::arg("epistasis_block_size") = 0, nb::arg("neutrality_mu") = 0,
             nb::arg("ruggedness_gamma") = 0);

    nb::class_<WModelOneMax, WModel>(m, "WModelOneMax",
                                     R"pbdoc(
            A W-model problem built on OneMax.
             
            This W-model problem applies four basic transformations: reduction of dummy variables,
            neutrality, epistasis, and fitness perturbation on the OneMax problem. Each transformation
            is configured by the corresponding argument dummy_select_rate, epistasis_block_size,
            neutrality_mu, and ruggedness_gamma.

            The definitions of OneMax and the transformations can be found in
            https://www.sciencedirect.com/science/article/pii/S1568494619308099

            We have acknowledged Thomas Weise, proposing the original W-model
            https://dl.acm.org/doi/abs/10.1145/3205651.3208240 and having discussions supporting this work.

            Reference
            ---------
            [WeiseW18] Thomas Weise and Zijun Wu. "Difficult features of combinatorial optimization problems
            and the tunable w-model benchmark problem for simulating them." Proc. of the Genetic and 
            Evolutionary Computation Conference Companion, 1769-1776, 2018

            [DoerrYHWSB20] Carola Doerr, Furong Ye, Naama Horesh, Hao Wang, Ofer M. Shir, Thomas Bäck.
            "Benchmarking discrete optimization heuristics with IOHprofiler."
            Applied Soft Computing, 88: 106027, 2020 

        )pbdoc")
        .def(nb::init<int, int, double, int, int, int>(), nb::arg("instance"), nb::arg("n_variables"),
             nb::arg("dummy_select_rate") = 0.0, nb::arg("epistasis_block_size") = 0, nb::arg("neutrality_mu") = 0,
             nb::arg("ruggedness_gamma") = 0);
}

void define_submodular_problems(nb::module_ &m)
{
    using namespace ioh::problem;
    using namespace ioh::problem::submodular;
    using ioh_python::integer::load_graph_instances;
    using ioh_python::integer::load_instances_doc;

    nb::class_<GraphConstraint, Constraint<int>>(m, "GraphConstraint").def("__repr__", &GraphConstraint::repr);

    nb::class_<pwt::PWTConstraint, Constraint<int>>(m, "PWTConstraint").def("__repr__", &pwt::PWTConstraint::repr);

    nb::class_<graph::Meta>(m, "GraphMeta", "Graph metadata")
        .def_ro("edge_file", &graph::Meta::edge_file, "File with edge data")
        .def_ro("edge_weights", &graph::Meta::edge_weights, "File with edge weights")
        .def_ro("vertex_weights", &graph::Meta::vertex_weights, "File with vertex weights")
        .def_ro("constraint_weights", &graph::Meta::constraint_weights, "File with constraint weights")
        .def_ro("constraint_variances", &graph::Meta::constraint_variances, "File with constraint variances")
        .def_ro("is_edge", &graph::Meta::is_edge, "Edge type flag")
        .def_ro("digraph", &graph::Meta::digraph, "Is digraph flag")
        .def_ro("constraint_limit", &graph::Meta::constraint_limit, "Maximum value of the constraint")
        .def_ro("n_vertices", &graph::Meta::n_vertices, "Number of vertices");

    nb::class_<graph::Graph>(m, "Graph", "Graph data structure")
        .def("__repr__", &graph::Graph::repr)
        .def("dimension", &graph::Graph::dimension, "Get the dimension of the graph")
        .def_ro("meta", &graph::Graph::meta, "Graph metadata")
        .def_ro("loaded", &graph::Graph::loaded, "Whether the graph is loaded in memory")
        .def_ro("adjacency_list", &graph::Graph::adjacency_list, "Adjacency list representation of the graph")
        .def_ro("edges", &graph::Graph::edges, "Edge list representation of the graph")
        .def_ro("constraint_weights", &graph::Graph::constraint_weights, "Constraint weights")
        .def_ro("constraint_variances", &graph::Graph::constraint_variances, "Constraint variances")
        .def_ro("edge_weights", &graph::Graph::edge_weights, "Edge weights")
        .def_ro("vertex_weights", &graph::Graph::vertex_weights, "Vertex weights");

    nb::class_<GraphProblem, IntegerSingleObjective>(m, "GraphProblem",
                                                     R"pbdoc(
            Graph-based problems (submodular problems)
             
            These submodular problems are all based on an underlying graph structure. 

            Reference
            ---------
            [Neumann23] Neumann, Frank, Aneta Neumann, Chao Qian, Viet Anh Do, Jacob de Nobel, Diederick Vermetten, 
            Saba Sadeghi Ahouei, Furong Ye, Hao Wang, and Thomas Bäck. 
            "Benchmarking Algorithms for Submodular Optimization Problems Using IOHProfiler." 
            arXiv preprint arXiv:2302.01464 (2023).

        )pbdoc")
        .def_static(
            "create",
            [](const std::string &name, int iid, int dim) {
                return ioh::common::Factory<GraphProblem, int, int>::instance().create(name, iid, dim);
            },
            nb::arg("problem_name"), nb::arg("instance_id"), nb::arg("dimension"),
            R"pbdoc(
                Create a problem instance

                Parameters
                ----------
                    problem_name: str
                        a string indicating the problem name. 
                    instance_id: int
                        an integer identifier of the problem instance
                    dimension: int
                        the dimensionality of the search space
            )pbdoc")
        .def_static(
            "create",
            [](int id, int iid, int dim) {
                return ioh::common::Factory<GraphProblem, int, int>::instance().create(id, iid, dim);
            },
            nb::arg("problem_id"), nb::arg("instance_id"), nb::arg("dimension"),
            R"pbdoc(
                Create a problem instance

                Parameters
                ----------
                    problem_name: int
                        a string indicating the problem name. 
                    instance_id: int
                        an integer identifier of the problem instance
                    dimension: int
                        the dimensionality of the search space
            )pbdoc")
        .def_prop_ro_static(
            "problems", [](nb::handle) { return ioh::common::Factory<GraphProblem, int, int>::instance().map(); },
            "All registered problems")
        .def_prop_ro(
            "graph", [](const GraphProblem &self) { return self.graph; }, "The underlying graph structure");

    // Python cannot construct these: their instances come from the factories
    nb::class_<MaxCut, GraphProblem>(m, "MaxCut", nb::is_final(), R"pbdoc(
            Max-Cut problems
             
            The maximum cut problem is a classical NP-hard problem and can be defined as follows. Given an undirected
            weighted graph G = (V, E, w) with weights w: E → R≥0 on the edges, the goal is to select a set V1 ⊆ V such that
            the sum of the weight of edges between V1 and V2 = V \ V1 is maximal.

            For a given search point x ∈ {0, 1}^n where n = |V|, we have V1(x) = {vi | xi = 1} and V2(x) = {vi | xi = 0}. 
            Let C(x) = {e ∈ E | e ∩ V1(x) 6= ∅ ∧ e ∩ V2(x) 6= ∅} be the cut of a given search point x. The goal is to maximize
            f'(x) = Sum_{e∈C(x)} w(e).

            Reference
            ---------
            [Neumann23] Neumann, Frank, Aneta Neumann, Chao Qian, Viet Anh Do, Jacob de Nobel, Diederick Vermetten, 
            Saba Sadeghi Ahouei, Furong Ye, Hao Wang, and Thomas Bäck. 
            "Benchmarking Algorithms for Submodular Optimization Problems Using IOHProfiler." 
            arXiv preprint arXiv:2302.01464 (2023).

        )pbdoc")
        .def_static("load_instances", &load_graph_instances<MaxCut>, nb::arg("path") = nb::none(),
                    load_instances_doc);

    nb::class_<MaxCoverage, GraphProblem>(m, "MaxCoverage", nb::is_final(),
                                          R"pbdoc(
            Max-Coverage problems
             
            The maximum coverage problem is a classical optimization problem on graphs. Given an undirected weighted
            graph G = (V, E, c) with costs c : V → R≥0 on the vertices. We denote by N(V') = 
            {vi | ∃e ∈ E : e ∩ V' != ∧ e ∩ vi != ∅} the set of all nodes of V' and their neighbors in G.
            For a given search point x ∈ {0, 1}^n where n = |V |, we have V'(x) = {vi | xi = 1} and 
            c(x) = Sum_{v∈V 0(x)} c(v).

            In the deterministic setting, the goal is to maximize f(x) = |N(V'(x))|
            under the constraint that c(x) ≤ B holds.

            Reference
            ---------
            [Neumann23] Neumann, Frank, Aneta Neumann, Chao Qian, Viet Anh Do, Jacob de Nobel, Diederick Vermetten, 
            Saba Sadeghi Ahouei, Furong Ye, Hao Wang, and Thomas Bäck. 
            "Benchmarking Algorithms for Submodular Optimization Problems Using IOHProfiler." 
            arXiv preprint arXiv:2302.01464 (2023).

        )pbdoc")
        .def_static("load_instances", &load_graph_instances<MaxCoverage>, nb::arg("path") = nb::none(),
                    load_instances_doc);

    nb::class_<MaxInfluence, GraphProblem>(m, "MaxInfluence", nb::is_final(),
                                           R"pbdoc(
            Max-Influence problems
             
            The maximum influence problem in social networks is an important submodular optimization problem that has been
            widely studied in the literature from various perspectives. Let a directed graph G(V, E) represent a social network, 
            where each node is a user and each edge (u, v) ∈ E has a probability pu,v representing the strength of influence from user u to v.

            A fundamental propagation model is independence cascade. Starting from a seed set X, it uses a set At to record the
            nodes activated at time t, and at time t + 1, each inactive neighbor v of u ∈ At becomes active with probability pu,v.
            This process is repeated until no nodes get activated at some time. The set of nodes activated by propagating from X
            is denoted as IC(X), which is a random variable.

            The goal is to maximize the expected value of IC(X). Note that the computation of the expected value is done by
            running a simulation of the influence process several times and averaging its results. In this sense, the computation of
            the objective function value is stochastic.

            For a given search point x ∈ {0, 1}^n where n = |V|, we have V'(x) = {vi | xi = 1} and c(x) = SUM_{v∈V 0(x)} c(v).


            Reference
            ---------
            [Neumann23] Neumann, Frank, Aneta Neumann, Chao Qian, Viet Anh Do, Jacob de Nobel, Diederick Vermetten, 
            Saba Sadeghi Ahouei, Furong Ye, Hao Wang, and Thomas Bäck. 
            "Benchmarking Algorithms for Submodular Optimization Problems Using IOHProfiler." 
            arXiv preprint arXiv:2302.01464 (2023).
        )pbdoc")
        .def_static("load_instances", &load_graph_instances<MaxInfluence>, nb::arg("path") = nb::none(),
                    load_instances_doc);

    nb::class_<PackWhileTravel, GraphProblem>(m, "PackWhileTravel", nb::is_final(),
                                              R"pbdoc(
            Packing while traveling problems
             
            The packing while traveling (PWT) problem is a non-monotone submodular optimization problem which is
            obtained from the traveling thief problem (TTP) when the route is fixed.

            The input is given as n + 1 cities with distances di, 1 ≤ i ≤ n, from city i to city i + 1. Each city i, 1 ≤ i ≤ n,
            contains a set of items Mi ⊆ M, |Mi| = mi. Each item eij ∈ Mi, 1 ≤ j ≤ mi, has a positive integer profit pij and
            weight wij . A fixed route N = (1, 2, ..., n + 1) is traveled by a vehicle with velocity v ∈ [vmin, vmax]. We denote by
            xij ∈ {0, 1} the variable indicating whether or not item eij is chosen in a solution 
            x = (x11, x12, ..., x1m1, x21, ..., xnmn) ∈ {0, 1}^m, where m = SUM_{n}^{i=1} mi. 
            The total benefit of selecting a subset of items selected by x is given as PWT(x) = P(x) − R · T(x),
            where P(x) is the total profit of the selected items and T(x) is the total travel time for the vehicle carrying the selected
            items. Formally, we have P(x) = Sum_{i=1}^n Sum_{j=1}^{mi} pij xij and
            T(x) = Sum_{i=1}^n di / (vmax − ν Sum_{k=1}^i Sum_{j=1}^{mk} wkj xkj
            Here, ν = (vmax−vmin) / B is a constant defined by the input parameters, where B is the capacity of the vehicle. The
            problem is already NP-hard without any additional constraints, but often considered with a typical knapsack
            constraint given as c(x) = Sum_{i=1}^n Sum_{j=1}^{mi} wij xij ≤ B.
            As fitness functions, we use g(x) = (f'(x), c(x)) with
            f'(x) = PWT(x) if c(x) ≤ B
            and f'(x) = B − c(x) − R · T(vmin) if c(x) > B
            where T(vmin) = (1 / vmin) Sum_i=1}^n di is the travel time at speed vmin.


            Reference
            ---------
            [Neumann23] Neumann, Frank, Aneta Neumann, Chao Qian, Viet Anh Do, Jacob de Nobel, Diederick Vermetten, 
            Saba Sadeghi Ahouei, Furong Ye, Hao Wang, and Thomas Bäck. 
            "Benchmarking Algorithms for Submodular Optimization Problems Using IOHProfiler." 
            arXiv preprint arXiv:2302.01464 (2023).
        )pbdoc")
        .def_static("load_instances", &load_graph_instances<PackWhileTravel>, nb::arg("path") = nb::none(),
                    load_instances_doc);
}
