#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "ioh.hpp"

#include <iostream>

namespace py = pybind11;
using namespace ioh::problem;

template <typename T>
void define_solution(py::module &m, const std::string &name)
{
    using Class = Solution<T>;

    py::options options;
    options.disable_function_signatures();

    py::class_<Class>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<std::vector<T>, double>(), py::arg("x"), py::arg("y"),
             R"pbdoc(
            A Solution object, which is a container for a search point and its corresponding fitness value.

            Parameters
            ----------
            x: list
                the search point in a search space, e.g., R^d or {0,1}^d
            y: float
                the corresponding objective value of `x`, i.e., y = f(x)

        )pbdoc")
        .def_readonly("x", &Class::x, "The search point in a search space, e.g., R^d or {0,1}^d")
        .def_readonly("y", &Class::y, "The corresponding objective value of `x`, i.e., y = f(x)")
        .def("__repr__", &Class::repr);
}

template <typename T>
void define_state(py::module &m, const std::string &name)
{
    using Class = State<T>;
    using Class2 = Solution<T>;
    py::class_<Class>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<Class2>(), py::arg("initial"),
             R"pbdoc(
                A state of a problem. Contains metrics such as the number of evaluations, and the current best value.

                Parameters
                ----------
                initial: Solution
                    an object to initialize the state, which contains a placeholder for a solution to an optimization problem. 
            )pbdoc")
        .def_readonly("evaluations", &Class::evaluations, "The number of times the problem has been evaluated so far.")
        .def_readonly("optimum_found", &Class::optimum_found,
                      "Boolean value indicating whether the optimum of a given problem has been found.")
        .def_readonly("current_best_internal", &Class::current_best_internal,
                      "The internal representation of the best so far solution. "
                      "See `current_internal` for a short explanation on the meaning of 'internal'")
        .def_readonly("current_best", &Class::current_best, "The current best-so-far solution.")
        .def_readonly("current_internal", &Class::current_internal,
                      "The internal representation of the last-evaluated solution. Note that, for a given "
                      "problem instance, the input search point will be transformed with an automorphism "
                      "and after evaluating the "
                      "transformed point, the resulting objective value will also be transformed with another "
                      "automorphism in the objective space. Such interal information are stored in this attribute.")
        .def_readonly("current", &Class::current, "The last-evaluated solution.")
        .def("__repr__", &Class::repr);
}

template <typename T>
void define_constraint(py::module &m, const std::string &name)
{
    using Class = Constraint<T>;

    py::options options;
    options.disable_function_signatures();

    py::class_<Class>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<std::vector<T>, std::vector<T>>(), py::arg("lb"), py::arg("ub"),
             R"pbdoc(
                Create box constraints. 

                Parameters
                ----------
                    lb: list
                        the lower bound of the search space/domain
                    ub: list
                        the upper bound of the search space/domain
            )pbdoc")
        .def(py::init<int, T, T>(), py::arg("size"), py::arg("lb"), py::arg("ub"),
             R"pbdoc(
                Create box constraints. 

                Parameters
                ----------
                    size: int
                        The size of the problem
                    lb: float
                        the lower bound of the search space/domain
                    ub: float
                        the upper bound of the search space/domain
            )pbdoc"

             )
        .def_readonly("ub", &Class::ub, "The upper bound (box constraint)")
        .def_readonly("lb", &Class::lb, "The lower bound (box constraint)")
        .def("__repr__", &Class::repr)
        .def("check", &Class::check,
             R"pbdoc(
                Check if a point is within the bounds or not.

                Parameters
                ----------
                    x: the search point to check
            )pbdoc");
}

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

template <typename P, typename T>
class PyProblem : public P
{
    [[nodiscard]] bool perform_registration()
    {
        auto meta_data = this->meta_data();
        auto constraint = this->constraint();
        auto &factory = ioh::common::Factory<P, int, int>::instance();
        factory.include(meta_data.name, meta_data.problem_id, [=](const int instance, const int n_variables) {
            return std::make_shared<PyProblem<P, T>>(MetaData(meta_data.problem_id, instance, meta_data.name,
                                                              n_variables, meta_data.optimization_type.type()),
                                                     constraint.resize(n_variables));
        });
        return true;
    }

public:
    explicit PyProblem(const MetaData &meta_data, const Constraint<T> &constraint) : P(meta_data, constraint) {}

    explicit PyProblem(const std::string &name, const int n_variables = 5, const int instance = 1,
                       const bool is_minimization = true, Constraint<T> constraint = Constraint<T>()) :
        P(MetaData(instance, name, n_variables,
                   is_minimization ? ioh::common::OptimizationType::Minimization
                                   : ioh::common::OptimizationType::Maximization),
          constraint)
    {

        auto registered = perform_registration();
    }

    double evaluate(const std::vector<T> &x) override { PYBIND11_OVERRIDE_PURE(double, P, evaluate, x); }

    [[nodiscard]] std::vector<T> transform_variables(std::vector<T> x) override
    {
        PYBIND11_OVERRIDE(std::vector<T>, P, transform_variables, x);
    }

    [[nodiscard]] double transform_objectives(const double y) override
    {
        PYBIND11_OVERRIDE(double, P, transform_objectives, y);
    }
};

template <typename ProblemType, typename T>
void define_base_class(py::module &m, const std::string &name)
{
    using PyProblem = PyProblem<ProblemType, T>;
    using Factory = ioh::common::Factory<ProblemType, int, int>;

    py::options options;
    options.disable_function_signatures();

    py::class_<ProblemType, PyProblem, std::shared_ptr<ProblemType>>(m, name.c_str(), py::buffer_protocol())
        .def(
            py::init<const std::string, int, int, bool, Constraint<T>>(), 
            py::arg("name"), 
            py::arg("n_variables") = 5,
            py::arg("instance") = 1, 
            py::arg("is_minimization") = true, 
            py::arg("constraint") = Constraint<T>(5),
            fmt::format("Base class for {} problems",  name).c_str()
            )
        .def("reset", &ProblemType::reset,
             R"pbdoc(
                Reset all state variables of the problem.
            )pbdoc")
        .def("attach_logger", &ProblemType::attach_logger,
             R"pbdoc(
                Attach a logger to the problem to allow performance tracking.

                Parameters
                ----------
                    logger: Logger
                        A logger-object from the IOHexperimenter `logger` module.
            )pbdoc")
        .def("detach_logger", &ProblemType::detach_logger,
             R"pbdoc(
                Remove the specified logger from the problem.
            )pbdoc")
        .def("__call__", &ProblemType::operator(),
             R"pbdoc(
                Evaluate the problem.

                Parameters
                ----------
                    x: list
                        the search point to evaluate. It must be a 1-dimensional array/list whose length matches search space's dimensionality
            )pbdoc")
        .def_static(
            "create",
            [](const std::string &name, int iid, int dim) { return Factory::instance().create(name, iid, dim); },
            py::arg("problem_name"), py::arg("instance_id"), py::arg("dimension"),
            R"pbdoc(
                Create a problem instance

                Parameters
                ----------
                    problem_name: a string indicating the problem name. For the built-in problems, we advise
                        the user to look into https://iohprofiler.github.io/IOHexp/Cpp/#using-individual-problems
                        for the full list of available problems
                    instance_id: an integer identifier of the problem instance, which seeds the random generation
                        of the tranformations in the search/objective spaces
                    dimension: integer, representing the dimensionality of the search space
            )pbdoc")
        .def_static(
            "create", [](int id, int iid, int dim) { return Factory::instance().create(id, iid, dim); },
            py::arg("problem_id"), py::arg("instance_id"), py::arg("dimension"),
            R"pbdoc(
                Create a problem instance

                Parameters
                ----------
                    problem_id: the index of the problem to create. For the built-in problems, the indexing of
                        problems can be found here: https://iohprofiler.github.io/IOHexp/Cpp/#using-individual-problems
                    instance_id: an integer identifier of the problem instance, which seeds the random generation
                        of the tranformations in the search/objective spaces
                    dimension: integer, representing the dimensionality of the search space
            )pbdoc")
        .def_property_readonly_static(
            "problems", [](py::object) { return Factory::instance().map(); }, "All registered problems")
        .def_property_readonly("log_info", &ProblemType::log_info, "Check what data is being sent to the logger.")
        .def_property_readonly(
            "state", &ProblemType::state,
            "The current state of the optimization process containing, e.g., the current solution and the "
            "number of function evaluated consumed so far")
        .def_property_readonly("meta_data", &ProblemType::meta_data,
                               "The static meta-data of the problem containing, e.g., problem id, instance id, and "
                               "problem's dimensionality")
        .def_property_readonly("objective", &ProblemType::objective,
                               "The optimum and its objective value for a problem instance")
        .def_property_readonly("constraint", &ProblemType::constraint, "The box constraints of the problem.")
        .def("__repr__", [=](const ProblemType &p) {
            using namespace ioh::common;
            const auto meta_data = p.meta_data();
            return "<" + name + fmt::format("Problem {:d}. ", meta_data.problem_id) + meta_data.name +
                fmt::format(" (iid={:d} dim={:d})>", meta_data.instance, meta_data.n_variables);
        });
}
static std::vector<py::handle> WRAPPED_FUNCTIONS;

bool register_python_fn(py::handle f)
{
    f.inc_ref();
    WRAPPED_FUNCTIONS.push_back(f);
    return true;
}


template <typename T>
void define_wrapper_functions(py::module &m, const std::string &class_name, const std::string &function_name)
{
    using WrappedProblem = WrappedProblem<T>;
    py::class_<WrappedProblem, Problem<T>, std::shared_ptr<WrappedProblem>>(m, class_name.c_str(),
                                                                            py::buffer_protocol());

    m.def(
        function_name.c_str(),
        [](py::handle f, const std::string &name, ioh::common::OptimizationType t, std::optional<double> lb,
           std::optional<double> ub, std::optional<py::handle> tx, std::optional<py::handle> ty,
           std::optional<py::handle> co) {
            register_python_fn(f);
            auto of = [f](const std::vector<T> &x) { return PyFloat_AsDouble(f(x).ptr()); };

            auto ptx = [tx](std::vector<T> x, const int iid) {
                if (tx)
                {
                    static bool r = register_python_fn(tx.value());
                    py::list px = (tx.value()(x, iid));
                    if (px.size() == x.size())
                        return px.cast<std::vector<T>>();
                    else
                        py::module_::import("warnings")
                            .attr("warn")(fmt::format("Objective transformation function returned an iterable of "
                                                      "invalid length ({} != {}). Transformation is disabled.",
                                                      px.size(), x.size()));
                }
                return x;
            };

            auto pty = [ty](double y, const int iid) {
                if (ty)
                {
                    static bool r = register_python_fn(ty.value());
                    return PyFloat_AsDouble(ty.value()(y, iid).ptr());
                }
                return y;
            };

            auto pco = [co, t](const int iid, const int dim) {
                if (co)
                {
                    static bool r = register_python_fn(co.value());
                    py::object xt = co.value()(iid, dim);
                    if (py::isinstance<py::iterable>(xt) && xt.cast<py::tuple>().size() == 2)
                    {
                        py::tuple args = xt;
                        py::list x = args[0];
                        py::float_ y = args[1];
                        return Solution<T>(x.cast<std::vector<T>>(), y.cast<double>());
                    }
                    return xt.cast<Solution<T>>();
                }
                return Solution<T>(dim, t);
            };

            wrap_function<T>(of, name, t, lb, ub, ptx, pty, pco);
        },
        py::arg("f"), py::arg("name"), py::arg("optimization_type") = ioh::common::OptimizationType::Minimization,
        py::arg("lb") = std::nullopt, py::arg("ub") = std::nullopt, py::arg("transform_variables") = std::nullopt,
        py::arg("transform_objectives") = std::nullopt, py::arg("calculate_objective") = std::nullopt);
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

void define_helper_classes(py::module &m)
{
    py::enum_<ioh::common::OptimizationType>(
        m, "OptimizationType", "Enum used for defining whether the problem is subject to minimization or maximization")
        .value("Maximization", ioh::common::OptimizationType::Maximization)
        .value("Minimization", ioh::common::OptimizationType::Minimization)
        .export_values();

    define_solution<double>(m, "RealSolution");
    define_solution<int>(m, "IntegerSolution");
    define_constraint<int>(m, "IntegerConstraint");
    define_constraint<double>(m, "RealConstraint");
    define_state<double>(m, "RealState");
    define_state<int>(m, "IntegerState");


    py::class_<MetaData>(m, "MetaData")
        .def(py::init<int, int, std::string, int, ioh::common::OptimizationType>(), py::arg("problem_id"),
             py::arg("instance"), py::arg("name"), py::arg("n_variables"), py::arg("optimization_type"),
             R"pbdoc(
                Problem meta data. Contains static information about problems, such as their name and id. 

                Parameters
                ----------
                problem_id: int
                    The id of the problem
                instance: int
                    The instance of the problem
                name: str
                    The name of the problem
                n_variables: int
                    The problem dimensionality
                optimization_type: OptimizationType
                    Enum value, denoting if this is a maximization or minimization problem    
               
            )pbdoc")
        .def_readonly("instance", &MetaData::instance, "The instance number of the current problem")
        .def_readonly("problem_id", &MetaData::problem_id, "The id of the problem within its suite")
        .def_readonly("name", &MetaData::name, "The name of the current problem")
        .def_readonly("optimization_type", &MetaData::optimization_type,
                      "The type of problem (maximization or minimization)")
        .def_readonly("n_variables", &MetaData::n_variables,
                      "The number of variables (dimension) of the current problem")
        .def("__repr__", &MetaData::repr);

    py::class_<ioh::logger::Info>(m, "LogInfo")
        .def(py::init<size_t, double, double, double, Solution<double>, Solution<double>>(), py::arg("evaluations"),
             py::arg("raw_y_best"), py::arg("transformed_y"), py::arg("transformed_y_best"), py::arg("current"),
             py::arg("optimum"),
             R"pbdoc(
                LogInfo struct. Gets passed to the call method of a logger when it is invoked. 

                Parameters
                ----------
                evaluations: int
                    The current number of evaluations
                raw_y_best: float
                    The raw current best fitnes value
                transformed_y: float
                    The transformed current fitness value
                transformed_y_best: float
                    The transformed current best fitness value
                current: Solution
                    The current solution passed to problem
                optimum: Solution
                    The optimum solution for the problem                   

            )pbdoc")
        .def_readonly("evaluations", &ioh::logger::Info::evaluations,
                      "The number of function evaluations performed on the current problem so far")
        .def_readonly("y_best", &ioh::logger::Info::raw_y_best, "The best fitness value found so far")
        .def_readonly("transformed_y", &ioh::logger::Info::transformed_y,
                      "The internal representation of the current fitness value")
        .def_readonly("transformed_y_best", &ioh::logger::Info::transformed_y_best,
                      "The internal representation of the best-so-far fitness")
        .def_readonly("current", &ioh::logger::Info::current, "The fitness of the last evaluated solution")
        .def_readonly("objective", &ioh::logger::Info::optimum, "The best possible fitness value");
}

void define_pbo_problems(py::module &m)
{
    py::class_<PBO, Integer, std::shared_ptr<PBO>>(m, "PBO",
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
            [](const std::string &name, int iid, int dim) { return ioh::common::Factory<PBO, int, int>::instance().create(name, iid, dim); },
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
            "create", [](int id, int iid, int dim) { return ioh::common::Factory<PBO, int, int>::instance().create(id, iid, dim); },
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
        ;

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
    py::class_<pbo::OneMaxDummy1, PBO, std::shared_ptr<pbo::OneMaxDummy1>>(m, "OneMaxDummy1", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::OneMaxDummy2, PBO, std::shared_ptr<pbo::OneMaxDummy2>>(m, "OneMaxDummy2", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::OneMaxNeutrality, PBO, std::shared_ptr<pbo::OneMaxNeutrality>>(m, "OneMaxNeutrality",
                                                                                   py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::OneMaxEpistasis, PBO, std::shared_ptr<pbo::OneMaxEpistasis>>(m, "OneMaxEpistasis", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::OneMaxRuggedness1, PBO, std::shared_ptr<pbo::OneMaxRuggedness1>>(m, "OneMaxRuggedness1",
                                                                                     py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::OneMaxRuggedness2, PBO, std::shared_ptr<pbo::OneMaxRuggedness2>>(m, "OneMaxRuggedness2",
                                                                                     py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::OneMaxRuggedness3, PBO, std::shared_ptr<pbo::OneMaxRuggedness3>>(m, "OneMaxRuggedness3",
                                                                                     py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::LeadingOnesDummy1, PBO, std::shared_ptr<pbo::LeadingOnesDummy1>>(m, "LeadingOnesDummy1",
                                                                                     py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::LeadingOnesDummy2, PBO, std::shared_ptr<pbo::LeadingOnesDummy2>>(m, "LeadingOnesDummy2",
                                                                                     py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::LeadingOnesNeutrality, PBO, std::shared_ptr<pbo::LeadingOnesNeutrality>>(m, "LeadingOnesNeutrality",
                                                                                             py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::LeadingOnesEpistasis, PBO, std::shared_ptr<pbo::LeadingOnesEpistasis>>(m, "LeadingOnesEpistasis",
                                                                                           py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::LeadingOnesRuggedness1, PBO, std::shared_ptr<pbo::LeadingOnesRuggedness1>>(
        m, "LeadingOnesRuggedness1", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::LeadingOnesRuggedness2, PBO, std::shared_ptr<pbo::LeadingOnesRuggedness2>>(
        m, "LeadingOnesRuggedness2", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::LeadingOnesRuggedness3, PBO, std::shared_ptr<pbo::LeadingOnesRuggedness3>>(
        m, "LeadingOnesRuggedness3", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::LABS, PBO, std::shared_ptr<pbo::LABS>>(m, "LABS", py::is_final(),
                                                           R"pbdoc(
            Low Autocorrelation Binary Sequences (LABS):
            x ↦ n^2 / 2∑_{k=1}^{n-1}(∑_{i=1}^{n−k}s_is_{i+k})^2, where s_i = 2x_i − 1

        )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::IsingRing, PBO, std::shared_ptr<pbo::IsingRing>>(m, "IsingRing", py::is_final(),
                                                                     R"pbdoc(
            Low Autocorrelation Binary Sequences (LABS):
            x ↦ n^2 / 2∑_{k=1}^{n-1}(∑_{i=1}^{n−k}s_is_{i+k})^2, where s_i = 2x_i − 1

        )pbdoc")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::IsingTorus, PBO, std::shared_ptr<pbo::IsingTorus>>(m, "IsingTorus", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::IsingTriangular, PBO, std::shared_ptr<pbo::IsingTriangular>>(m, "IsingTriangular", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::MIS, PBO, std::shared_ptr<pbo::MIS>>(m, "MIS", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::NQueens, PBO, std::shared_ptr<pbo::NQueens>>(m, "NQueens", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<pbo::ConcatenatedTrap, PBO, std::shared_ptr<pbo::ConcatenatedTrap>>(m, "ConcatenatedTrap",
                                                                                   py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
}

void define_bbob_problems(py::module &m)
{
    py::class_<BBOB, Real, std::shared_ptr<BBOB>>(m, "BBOB",
                                                  R"pbdoc(
            Black-Box Optimization Benchmarking (BBOB) problem set.

            Contains 24 noiselessreal-valued test functions supported on [-5, 5]^n, where n is the dimensionality.

            This problem was orginally proposed by Hansen et. al. in [FinckHRA10] and was implemented
            as the core component of the COmparing Continous Optimizer (COCO) platform [HansenARMTB20].

            We took the implementation of those 24 functions in
            https://github.com/numbbo/coco/tree/master/code-experiments/src (v2.2)
            and adopted those to our framework.

            We have acknowledged and specified in our license file
            https://github.com/IOHprofiler/IOHexperimenter/blob/master/LICENSE.md
            the usage and modification to the COCO/BBOB sources.

            Reference
            ---------
            [HansenARMTB20] Nikolaus Hansen, Anne Auger, Raymond Ros, Olaf Mersmann,
            Tea Tusar, and Dimo Brockhoff. "COCO: A platform for comparing continuous optimizers in
            a black-box setting." Optimization Methods and Software (2020): 1-31.

            [FinckHRA10] Steffen Finck, Nikolaus Hansen, Raymond Ros, and Anne Auger.
            "Real-parameter black-box optimization benchmarking 2009: Presentation of the noiseless functions."
            Technical Report 2009/20, Research Center PPE, 2009. Updated February, 2010.

        )pbdoc")
        .def_static(
            "create",
            [](const std::string &name, int iid, int dim) { return ioh::common::Factory<BBOB, int, int>::instance().create(name, iid, dim); },
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
            "create", [](int id, int iid, int dim) { return ioh::common::Factory<BBOB, int, int>::instance().create(id, iid, dim); },
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
            ;
    py::class_<bbob::Sphere, BBOB, std::shared_ptr<bbob::Sphere>>(m, "Sphere", py::is_final(), "Sphere function")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Ellipsoid, BBOB, std::shared_ptr<bbob::Ellipsoid>>(m, "Ellipsoid", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Rastrigin, BBOB, std::shared_ptr<bbob::Rastrigin>>(m, "Rastrigin", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::BuecheRastrigin, BBOB, std::shared_ptr<bbob::BuecheRastrigin>>(m, "BuecheRastrigin",
                                                                                    py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::LinearSlope, BBOB, std::shared_ptr<bbob::LinearSlope>>(m, "LinearSlope", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::AttractiveSector, BBOB, std::shared_ptr<bbob::AttractiveSector>>(m, "AttractiveSector",
                                                                                      py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::StepEllipsoid, BBOB, std::shared_ptr<bbob::StepEllipsoid>>(m, "StepEllipsoid", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Rosenbrock, BBOB, std::shared_ptr<bbob::Rosenbrock>>(m, "Rosenbrock", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::RosenbrockRotated, BBOB, std::shared_ptr<bbob::RosenbrockRotated>>(m, "RosenbrockRotated",
                                                                                        py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::EllipsoidRotated, BBOB, std::shared_ptr<bbob::EllipsoidRotated>>(m, "EllipsoidRotated",
                                                                                      py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Discus, BBOB, std::shared_ptr<bbob::Discus>>(m, "Discus", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::BentCigar, BBOB, std::shared_ptr<bbob::BentCigar>>(m, "BentCigar", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::SharpRidge, BBOB, std::shared_ptr<bbob::SharpRidge>>(m, "SharpRidge", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::DifferentPowers, BBOB, std::shared_ptr<bbob::DifferentPowers>>(m, "DifferentPowers",
                                                                                    py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::RastriginRotated, BBOB, std::shared_ptr<bbob::RastriginRotated>>(m, "RastriginRotated",
                                                                                      py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Weierstrass, BBOB, std::shared_ptr<bbob::Weierstrass>>(m, "Weierstrass", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Schaffers10, BBOB, std::shared_ptr<bbob::Schaffers10>>(m, "Schaffers10", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Schaffers1000, BBOB, std::shared_ptr<bbob::Schaffers1000>>(m, "Schaffers1000", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::GriewankRosenBrock, BBOB, std::shared_ptr<bbob::GriewankRosenBrock>>(m, "GriewankRosenBrock",
                                                                                          py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Schwefel, BBOB, std::shared_ptr<bbob::Schwefel>>(m, "Schwefel", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Gallagher101, BBOB, std::shared_ptr<bbob::Gallagher101>>(m, "Gallagher101", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Gallagher21, BBOB, std::shared_ptr<bbob::Gallagher21>>(m, "Gallagher21", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Katsuura, BBOB, std::shared_ptr<bbob::Katsuura>>(m, "Katsuura", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::LunacekBiRastrigin, BBOB, std::shared_ptr<bbob::LunacekBiRastrigin>>(m, "LunacekBiRastrigin",
                                                                                          py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
}

void define_problem_bases(py::module &m)
{
    define_base_class<Real, double>(m, "Real");
    define_base_class<Integer, int>(m, "Integer");
    define_wrapper_functions<double>(m, "RealWrappedProblem", "wrap_real_problem");
    define_wrapper_functions<int>(m, "IntegerWrappedProblem", "wrap_integer_problem");
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
    py::class_<WModel, WModelTrampoline, Integer, std::shared_ptr<WModel>>(m, "AbstractWModel",
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

void define_problem(py::module &m)
{
    define_problem_bases(m);
    define_bbob_problems(m);
    define_pbo_problems(m);
    define_wmodels(m);

    py::module_::import("atexit").attr("register")(py::cpp_function([]() {
        for (const auto fn : WRAPPED_FUNCTIONS)
        {
            if (fn)
                fn.dec_ref();
        }
    }));
}
