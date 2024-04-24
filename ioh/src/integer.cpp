#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "ioh.hpp"

namespace py = pybind11;
using namespace ioh::problem;


void define_pbo_problems(py::module &m)
{
    py::class_<PBO, IntegerSingleObjective, std::shared_ptr<PBO>>(m, "PBO",
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
            py::arg("problem_name"), py::arg("instance_id"), py::arg("dimension"),
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
                return ioh::common::Factory<PBO, int, int>::instance().create(id, iid, dim);
            },
            py::arg("problem_id"), py::arg("instance_id"), py::arg("dimension"),
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
        .def_property_readonly_static(
            "problems", [](py::object) { return ioh::common::Factory<PBO, int, int>::instance().map(); },
            "All registered problems");

    py::class_<pbo::OneMax, PBO, std::shared_ptr<pbo::OneMax>>(m, "OneMax", py::is_final(),
                                                               R"pbdoc(
            OneMax:
            {0,1}^n → [0..n], x ↦ ∑_{i=1}^n x_i.

        )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::LeadingOnes, PBO, std::shared_ptr<pbo::LeadingOnes>>(m, "LeadingOnes", py::is_final(),
                                                                         R"pbdoc(
            LeadingOnes:
            {0,1}^n → [0..n], x ↦ max{i∈[0..n] ∣ ∀j≤i: x_j=1}

        )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::Linear, PBO, std::shared_ptr<pbo::Linear>>(m, "Linear", py::is_final(),
                                                               R"pbdoc(
            A Linear Function with Harmonic Weights:
            {0,1}^n → ℝ, x ↦ ∑_i i * x_i
        )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::OneMaxDummy1, PBO, std::shared_ptr<pbo::OneMaxDummy1>>(m, "OneMaxDummy1", py::is_final(),
                                                                           R"pbdoc(
                A variant of OneMax applying the Dummy transformation of W-model. m = 0.5n.
                Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
            )pbdoc")

        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::OneMaxDummy2, PBO, std::shared_ptr<pbo::OneMaxDummy2>>(m, "OneMaxDummy2", py::is_final(),
                                                                           R"pbdoc(
                A variant of OneMax applying the Dummy transformation of W-model. m = 0.9n.
                Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
            )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::OneMaxNeutrality, PBO, std::shared_ptr<pbo::OneMaxNeutrality>>(m, "OneMaxNeutrality",
                                                                                   py::is_final(),
                                                                                   R"pbdoc(
                A variant of OneMax applying the Neutrality transformation of W-model. \mu = 3.
                Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::OneMaxEpistasis, PBO, std::shared_ptr<pbo::OneMaxEpistasis>>(m, "OneMaxEpistasis", py::is_final(),
                                                                                 R"pbdoc(
                A variant of OneMax applying the Epistasis transformation of W-model. \nu = 4.
                Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::OneMaxRuggedness1, PBO, std::shared_ptr<pbo::OneMaxRuggedness1>>(m, "OneMaxRuggedness1",
                                                                                     py::is_final(),
                                                                                     R"pbdoc(
                A variant of OneMax applying the first Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::OneMaxRuggedness2, PBO, std::shared_ptr<pbo::OneMaxRuggedness2>>(m, "OneMaxRuggedness2",
                                                                                     py::is_final(),
                                                                                     R"pbdoc(
                A variant of OneMax applying the second Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::OneMaxRuggedness3, PBO, std::shared_ptr<pbo::OneMaxRuggedness3>>(m, "OneMaxRuggedness3",
                                                                                     py::is_final(),
                                                                                     R"pbdoc(
                A variant of OneMax applying the third Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::LeadingOnesDummy1, PBO, std::shared_ptr<pbo::LeadingOnesDummy1>>(m, "LeadingOnesDummy1",
                                                                                     py::is_final(),
                                                                                     R"pbdoc(
                A variant of LeadingOnes applying the Dummy transformation of W-model. m = 0.5n.
                Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
            )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::LeadingOnesDummy2, PBO, std::shared_ptr<pbo::LeadingOnesDummy2>>(m, "LeadingOnesDummy2",
                                                                                     py::is_final(),
                                                                                     R"pbdoc(
                A variant of LeadingOnes applying the Dummy transformation of W-model. m = 0.9n.
                Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
            )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::LeadingOnesNeutrality, PBO, std::shared_ptr<pbo::LeadingOnesNeutrality>>(m, "LeadingOnesNeutrality",
                                                                                             py::is_final(),
                                                                                             R"pbdoc(
                A variant of LeadingOnes applying the Neutrality transformation of W-model. \mu = 3.
                Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::LeadingOnesEpistasis, PBO, std::shared_ptr<pbo::LeadingOnesEpistasis>>(m, "LeadingOnesEpistasis",
                                                                                           py::is_final(),
                                                                                           R"pbdoc(
                A variant of LeadingOnes applying the Epistasis transformation of W-model. \nu = 4.
                Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::LeadingOnesRuggedness1, PBO, std::shared_ptr<pbo::LeadingOnesRuggedness1>>(
        m, "LeadingOnesRuggedness1", py::is_final(),
        R"pbdoc(
                A variant of LeadingOnes applying the first Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::LeadingOnesRuggedness2, PBO, std::shared_ptr<pbo::LeadingOnesRuggedness2>>(
        m, "LeadingOnesRuggedness2", py::is_final(),
        R"pbdoc(
                A variant of LeadingOnes applying the second Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::LeadingOnesRuggedness3, PBO, std::shared_ptr<pbo::LeadingOnesRuggedness3>>(
        m, "LeadingOnesRuggedness3", py::is_final(),
        R"pbdoc(
                A variant of LeadingOnes applying the third Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
                )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::LABS, PBO, std::shared_ptr<pbo::LABS>>(m, "LABS", py::is_final(),
                                                           R"pbdoc(
            Low Autocorrelation Binary Sequences (LABS):
            x ↦ n^2 / 2∑_{k=1}^{n-1}(∑_{i=1}^{n−k}s_is_{i+k})^2, where s_i = 2x_i − 1

        )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::IsingRing, PBO, std::shared_ptr<pbo::IsingRing>>(m, "IsingRing", py::is_final(),
                                                                     R"pbdoc(
            The Ising Spin Glass model arose in solid-state physics and statistical mechanics, aiming to 
            describe simple interactions within many-particle systems. A compact form used here is 
            x ↦ ∑_{{u,v} \in E}[x_ux_v - (1-x_u) (1-x_v)]. E is defined over a one-dimensional lattice.
            Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.

        )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::IsingTorus, PBO, std::shared_ptr<pbo::IsingTorus>>(m, "IsingTorus", py::is_final(),
                                                                       R"pbdoc(
            The Ising Spin Glass model arose in solid-state physics and statistical mechanics, aiming to 
            describe simple interactions within many-particle systems. A compact form used here is 
            x ↦ ∑_{{u,v} \in E}[x_ux_v - (1-x_u) (1-x_v)]. E is defined over a two-dimensional lattice.
            Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.
        )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::IsingTriangular, PBO, std::shared_ptr<pbo::IsingTriangular>>(m, "IsingTriangular", py::is_final(),
                                                                                 R"pbdoc(
            The Ising Spin Glass model arose in solid-state physics and statistical mechanics, aiming to 
            describe simple interactions within many-particle systems. A compact form used here is 
            x ↦ ∑_{{u,v} \in E}[x_ux_v - (1-x_u) (1-x_v)]. E is defined over a three-dimensional lattice.
            Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.
        )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::MIS, PBO, std::shared_ptr<pbo::MIS>>(m, "MIS", py::is_final(),
                                                         R"pbdoc(
            The maximum independent vertex set (MIVS) formulated as 
            x ↦ ∑_i x_i - n ∑_{i,j} x_i x_j e_{i,j}, where e_{i,j} = 1 if {i,j} \in E, otherwise e_{i,j} = 0.
            Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.                       
        )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::NQueens, PBO, std::shared_ptr<pbo::NQueens>>(m, "NQueens", py::is_final(),
                                                                 R"pbdoc(
            The N-queens problem (NQP) is defined as the task to place N queens on an N*N chessboard in such a 
            way that they cannot attack each other.   
            Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.                      
        )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::ConcatenatedTrap, PBO, std::shared_ptr<pbo::ConcatenatedTrap>>(m, "ConcatenatedTrap",
                                                                                   py::is_final(),
                                                                                   R"pbdoc(
        Concatenated Trap (CT) is defined by partitioning a bit-string into segments of length k and 
        concatenating m = n/k trap functions that takes each segment as input. The trap function is 
        defined as follows: f_trap(u) = 1 if the k number u of ones satisfies u = k and 
        f_trap(u) = (k−1−u)/k otherwise. We use k = 5 in our experiments.
        Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.
        )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
}


class WModelTrampoline : public WModel
{
public:
    using WModel::WModel;

protected:
    int wmodel_evaluate(const std::vector<int> &x) override { PYBIND11_OVERRIDE_PURE(int, WModel, wmodel_evaluate, x); }
};

class WModelPublisher : public WModel
{
public:
    using WModel::WModel::wmodel_evaluate;
};

void define_wmodels(py::module &m)
{
    py::class_<WModel, WModelTrampoline, IntegerSingleObjective, std::shared_ptr<WModel>>(m, "AbstractWModel",
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
        .def(py::init<int, int, int, std::string, double, int, int, int>(), py::arg("problem_id"), py::arg("instance"),
             py::arg("n_variables"), py::arg("name"), py::arg("dummy_select_rate") = 0.0,
             py::arg("epistasis_block_size") = 0, py::arg("neutrality_mu") = 0, py::arg("ruggedness_gamma") = 0)
        .def("wmodel_evaluate", &WModelPublisher::wmodel_evaluate);

    py::class_<wmodel::WModelLeadingOnes, WModel, std::shared_ptr<wmodel::WModelLeadingOnes>>(m, "WModelLeadingOnes",
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
        .def(py::init<int, int, double, int, int, int>(), py::arg("instance"), py::arg("n_variables"),
             py::arg("dummy_select_rate") = 0.0, py::arg("epistasis_block_size") = 0, py::arg("neutrality_mu") = 0,
             py::arg("ruggedness_gamma") = 0);

    py::class_<wmodel::WModelOneMax, WModel, std::shared_ptr<wmodel::WModelOneMax>>(m, "WModelOneMax",
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
        .def(py::init<int, int, double, int, int, int>(), py::arg("instance"), py::arg("n_variables"),
             py::arg("dummy_select_rate") = 0.0, py::arg("epistasis_block_size") = 0, py::arg("neutrality_mu") = 0,
             py::arg("ruggedness_gamma") = 0);
}

void define_submodular_problems(py::module &m)
{
    using namespace ioh::problem;
    using namespace submodular;

    py::class_<GraphConstraint, Constraint<int>, std::shared_ptr<GraphConstraint>>(m, "GraphConstraint")
        .def("__repr__", &GraphConstraint::repr);

    py::class_<pwt::PWTConstraint, Constraint<int>, std::shared_ptr<pwt::PWTConstraint>>(m, "PWTConstraint")
        .def("__repr__", &pwt::PWTConstraint::repr);


    py::class_<GraphProblem, IntegerSingleObjective, std::shared_ptr<GraphProblem>>(m, "GraphProblem",
                                                                                    "Graph type problem",
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
            py::arg("problem_name"), py::arg("instance_id"), py::arg("dimension"),
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
            py::arg("problem_id"), py::arg("instance_id"), py::arg("dimension"),
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
        .def_property_readonly_static(
            "problems", [](py::object) { return ioh::common::Factory<GraphProblem, int, int>::instance().map(); },
            "All registered problems");


    py::class_<MaxCut, GraphProblem, std::shared_ptr<MaxCut>>(m, "MaxCut", py::is_final(), R"pbdoc(
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
        .def_static("load_instances", &GraphProblemType<MaxCut>::load_graph_instances<int, int>,
                    py::arg("path") = std::nullopt);

    // Don't allow these object to be created in from constructors in python
    py::class_<MaxCoverage, GraphProblem, std::shared_ptr<MaxCoverage>>(m, "MaxCoverage", py::is_final(),
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
        .def_static("load_instances", &GraphProblemType<MaxCoverage>::load_graph_instances<int, int>,
                    py::arg("path") = std::nullopt);


    py::class_<MaxInfluence, GraphProblem, std::shared_ptr<MaxInfluence>>(m, "MaxInfluence", py::is_final(),
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
        .def_static("load_instances", &GraphProblemType<MaxInfluence>::load_graph_instances<int, int>,
                    py::arg("path") = std::nullopt);

    py::class_<PackWhileTravel, GraphProblem, std::shared_ptr<PackWhileTravel>>(m, "PackWhileTravel", py::is_final(),
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
        .def_static("load_instances", &GraphProblemType<PackWhileTravel>::load_graph_instances<int, int>,
                    py::arg("path") = std::nullopt);
}