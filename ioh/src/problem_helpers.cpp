#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "ioh.hpp"

namespace py = pybind11;
using namespace ioh::problem;



template <typename T, typename R>
void define_solution(py::module &m, const std::string &name)
{
    using Class = Solution<T, R>;

    py::options options;
    options.disable_function_signatures();

    py::class_<Class>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<std::vector<T>, R>(), py::arg("x"), py::arg("y"),
             R"pbdoc(
            A Solution object, which is a container for a search point and its corresponding fitness value.

            Parameters
            ----------
            x: list
                the search point in a search space, e.g., R^d or {0,1}^d
            y: float | list
                the corresponding objective value of `x`, i.e., y = f(x)

        )pbdoc")
        .def_property(
            "x", [](const Class &c) { return py::array(c.x.size(), c.x.data()); },
            [](Class &self, const std::vector<T> &x) { self.x = x; },
            "The search point in a search space, e.g., R^d or {0,1}^d")
        .def_readonly("y", &Class::y, "The corresponding objective value of `x`, i.e., y = f(x)")
        .def("__repr__", &Class::repr);
}

template <typename T>
void define_state(py::module &m, const std::string &name)
{
    using Class = State<T, double>;
    using Class2 = Solution<T, double>;
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
                      "Boolean value indicating whether final_target_found optimum of a given problem has been found.")
        .def_readonly("final_target_found", &Class::final_target_found,
                      "Boolean value indicating whether the final target of a given problem has been found.")
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
        .def_readonly("y_unconstrained", &Class::y_unconstrained, "The current unconstrained value.")
        .def_readonly("y_unconstrained_best", &Class::y_unconstrained_best, "The current best unconstrained value.")
        .def_readonly("has_improved", &Class::has_improved,
                      "Whether the last call to problem has caused global improvement.")
        .def("__repr__", &Class::repr);
}

template <typename T>
void define_multi_state(py::module &m, const std::string &name)
{
    using Class = State<T, std::vector<double>>;
    using Class2 = Solution<T, std::vector<double>>;
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
                      "Boolean value indicating whether final_target_found optimum of a given problem has been found.")
        .def_readonly("final_target_found", &Class::final_target_found,
                      "Boolean value indicating whether the final target of a given problem has been found.")
        .def_readonly("current", &Class::current,
                      "The internal representation of the current solution.")
        .def_readonly("pareto_front", &Class::pareto_front,
                      "The internal representation of the current pareto front.")
        .def_readonly("has_improved", &Class::has_improved,
                      "Whether the last call to problem has caused global improvement.")
        .def("__repr__", &Class::repr);
}




template <typename T>
struct PyConstraint : Constraint<T>
{
    using Constraint<T>::Constraint;

    bool compute_violation(const std::vector<T> &x) override
    {
        PYBIND11_OVERRIDE_PURE(bool, Constraint<T>, compute_violation, py::array(x.size(), x.data()));
    }

    [[nodiscard]] std::string repr() const override { return "<AbstractConstraint>"; }
};

template <typename T>
void define_constraint(py::module &m, const std::string &name)
{
    using Class = Constraint<T>;

    py::class_<Class, PyConstraint<T>, std::shared_ptr<Class>>(m, name.c_str(), py::buffer_protocol())
        .def_readwrite("enforced", &Class::enforced, "The type of constraint enforcement applied.")
        .def_readwrite("weight", &Class::weight, "The weight given to this constraint")
        .def_readwrite("exponent", &Class::exponent, "The exponent for this constraint")
        .def("__call__", &Class::operator())
        .def("is_feasible", &Class::is_feasible)
        .def("compute_violation", &Class::compute_violation)
        .def("penalize", &Class::penalize)
        .def("violation", &Class::violation)
        .def("penalty", &Class::penalty);
}

template <typename T>
void define_constraintset(py::module &m, const std::string &name)
{
    using Class = ConstraintSet<T>;

    py::class_<Class, std::shared_ptr<Class>>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<Constraints<T>>(), py::arg("constraints") = Constraints<T>{},
             R"pbdoc(
                A set of constraints

                Parameters
                ----------
                    constraints: list[Constraint]
                        The list of constraints
                )pbdoc")
        .def("add", &Class::add)
        .def("remove", &Class::remove)
        .def("penalize", &Class::penalize)
        .def("penalty", &Class::penalty)
        .def("violation", &Class::violation)
        .def("n", &Class::n)
        .def("__getitem__", &Class::operator[])
        .def("__repr__", &Class::repr);
}


bool register_python_fn(py::handle f);

template <typename T>
void define_functionalconstraint(py::module &m, const std::string &name)
{
    using Parent = Constraint<T>;
    using Class = FunctionalConstraint<T>;

    py::class_<Class, Parent, std::shared_ptr<Class>>(m, name.c_str(), py::buffer_protocol())
        .def(py::init(
                 [](py::handle f, const double w, const double ex, const constraint::Enforced e, const std::string &n) {
                     register_python_fn(f);
                     auto fn = [f](const std::vector<T> &x) {
                         return PyFloat_AsDouble(f(py::array(x.size(), x.data())).ptr());
                     };
                     return Class(fn, w, ex, e, n);
                 }),
             py::arg("fn"), py::arg("weight") = 1.0, py::arg("exponent") = 1.0,
             py::arg("enforced") = constraint::Enforced::SOFT, py::arg("name") = "",
             R"pbdoc(
                General Constraint, defined by a function

                Parameters
                ----------
                    f: list -> float
                        A function that computes the violation of a point
                    weight: float
                        The weight given to the violation when penalizing the objective function
                    exponent: float
                        The exponent given to the violation when penalizing the objective function
                    enforced: ConstraintEnforcement
                        how the constraint should be enforced
                    name: str
                        name of the constraint                    
            )pbdoc")
        .def("__repr__", &Class::repr);
}


template <typename T>
void define_bounds(py::module &m, const std::string &name)
{

    using Parent = Constraint<T>;
    using Class = Bounds<T>;

    py::options options;
    options.disable_function_signatures();

    py::class_<Class, Parent, std::shared_ptr<Class>>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<std::vector<T>, std::vector<T>, constraint::Enforced>(), py::arg("lb"), py::arg("ub"),
             py::arg("enforced") = constraint::Enforced::NOT,
             R"pbdoc(
                Problem Bounds, defined as a Box Constraint

                Parameters
                ----------
                    lb: list
                        the lower bound of the search space/domain
                    ub: list
                        the upper bound of the search space/domain
                    enforced: ConstraintEnforcement
                        whether the constraint should be enforced
                    
            )pbdoc")
        .def(py::init<int, T, T, constraint::Enforced>(), py::arg("size"), py::arg("lb"), py::arg("ub"),
             py::arg("enforced") = constraint::Enforced::NOT,
             R"pbdoc(
                Problem Bounds, defined as a Box Constraint

                Parameters
                ----------
                    size: int
                        The size of the problem
                    lb: float
                        the lower bound of the search space/domain
                    ub: float
                        the upper bound of the search space/domain
                    enforced: ConstraintEnforcement
                        whether the constraint should be enforced
            )pbdoc"

             )
        .def_property(
            "ub", [](const Class &c) { return py::array(c.ub.size(), c.ub.data()); },
            [](Class &c, const std::vector<T> &vec) { c.ub = vec; }, "The upper bound (box constraint)")
        .def_property(
            "lb", [](const Class &c) { return py::array(c.lb.size(), c.lb.data()); },
            [](Class &c, const std::vector<T> &vec) { c.lb = vec; }, "The lower bound (box constraint)")
        .def("__repr__", &Class::repr)
        .def("compute_violation", &Class::compute_violation,
             R"pbdoc(
                Check if a point is within the bounds or not.

                Parameters
                ----------
                    x: list | np.ndarray
                        the search point to check
                    y: float
                        the objective value
                Returns
                -------
                bool
                    Whether there is a constraint violation
            )pbdoc");
}

template <typename T>
void define_constraint_types(py::module &m, const std::string &name)
{
    define_constraint<T>(m, fmt::format("Abstract{}Constraint", name));
    define_constraintset<T>(m, fmt::format("{}ConstraintSet", name));
    define_functionalconstraint<T>(m, fmt::format("{}Constraint", name));
    define_bounds<T>(m, fmt::format("{}Bounds", name));
}


void define_helper_classes(py::module &m)
{
    py::enum_<ioh::common::OptimizationType>(
        m, "OptimizationType", "Enum used for defining whether the problem is subject to minimization or maximization")
        .value("MAX", ioh::common::OptimizationType::MAX)
        .value("MIN", ioh::common::OptimizationType::MIN)
        .export_values();

    py::enum_<ioh::problem::constraint::Enforced>(m, "ConstraintEnforcement",
                                                  "Enum defining constraint handling strategies.")
        .value("NOT", ioh::problem::constraint::Enforced::NOT, "Do not calculate the constraint at all")
        .value("HIDDEN", ioh::problem::constraint::Enforced::HIDDEN,
               "Do not enforce, but still calculate to enable logging.")
        .value("SOFT", ioh::problem::constraint::Enforced::SOFT,
               "Penalize (y + p), but aggregate all the constraint penalties into a sum ")
        .value("HARD", ioh::problem::constraint::Enforced::HARD,
               "Penalize (p only), and if violation return only the penalty for this constraint in contraintset")
        .value("OVERRIDE", ioh::problem::constraint::Enforced::OVERRIDE,
               "Penalize (p only), and if violation return the custom penalization function this constraint in "
               "contraintset")
        .export_values();

    define_solution<double, double>(m, "RealSolution");
    define_solution<int, double>(m, "IntegerSolution");
    define_solution<double, std::vector<double>>(m, "MultiRealSolution");
    define_solution<int, std::vector<double>>(m, "MultiIntegerSolution");
    define_constraint_types<int>(m, "Integer");
    define_constraint_types<double>(m, "Real");
    define_state<double>(m, "RealState");
    define_state<int>(m, "IntegerState");
    define_multi_state<double>(m, "MultiRealState");
    define_multi_state<int>(m, "MultiIntegerState");

    py::class_<MetaData>(m, "MetaData")
        .def(py::init<int, int, std::string, int, int, ioh::common::OptimizationType, double>(), py::arg("problem_id"),
             py::arg("instance"), py::arg("name"), py::arg("n_variables"), py::arg("n_objectives"), py::arg("optimization_type"),
             py::arg("final_target") = 1e-8,
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
                n_objectives: int
                    The number of objectives of the problem
                optimization_type: OptimizationType
                    Enum value, denoting if this is a maximization or minimization problem
                final_target: float
                    The final target of the function    
               
            )pbdoc")
        .def_readonly("instance", &MetaData::instance, "The instance number of the current problem")
        .def_readonly("problem_id", &MetaData::problem_id, "The id of the problem within its suite")
        .def_readonly("name", &MetaData::name, "The name of the current problem")
        .def_property_readonly(
            "optimization_type", [](const MetaData &meta) { return meta.optimization_type.type(); },
            "The type of problem (maximization or minimization)")
        .def_readonly("n_variables", &MetaData::n_variables,
                      "The number of variables (dimension) of the current problem")
        .def_readonly("n_objectives", &MetaData::n_objectives,
                      "The number of objectives of the current problem")
        .def_readonly("final_target", &MetaData::final_target)
        .def("__repr__", &MetaData::repr)
        .def("__eq__", &MetaData::operator==);
    

    using SingleObjectiveInfo = ioh::logger::Info<double>;
    using MultiObjectiveInfo = ioh::logger::Info<std::vector<double>>;

    py::class_<SingleObjectiveInfo>(m, "LogInfo")
        .def(py::init<size_t, double, double, double, double, double, double, std::vector<double>, std::vector<double>,
                      std::vector<double>, Solution<double, double>, bool>(),
             py::arg("evaluations"), py::arg("raw_y"), py::arg("raw_y_best"), py::arg("transformed_y"),
             py::arg("transformed_y_best"), py::arg("y"), py::arg("y_best"), py::arg("x"), py::arg("violations"),
             py::arg("penalties"), py::arg("optimum"), py::arg("has_improved") = false,
             R"pbdoc(
                LogInfo struct. Gets passed to the call method of a logger when it is invoked. 

                Parameters
                ----------
                evaluations: int
                    The current number of evaluations
                y_best: float
                    The raw current fitness value
                raw_y_best: float
                    The raw current best fitness value
                transformed_y: float
                    The transformed current fitness value
                transformed_y_best: float
                    The transformed current best fitness value
                y: float
                    The transformed current fitness value with constraints applied
                y_best: float
                    The transformed current best fitness value with constraints applied
                x: np.npdarray | list
                    The current solution passed to problem
                violation: np.npdarray | list
                    The current solution constraint violations
                penalties: np.npdarray | list
                    The current solution constraint penalties
                optimum: Solution
                    The optimum solution for the problem
                has_improved: bool = True
                    Whether the evaluation has caused an improvement in the objective value

            )pbdoc")
        .def_readonly("evaluations", &SingleObjectiveInfo::evaluations,
                      "The number of function evaluations performed on the current problem so far")
        .def_readonly("raw_y", &SingleObjectiveInfo::raw_y)
        .def_readonly("raw_y_best", &SingleObjectiveInfo::raw_y_best)

        .def_readonly("transformed_y", &SingleObjectiveInfo::transformed_y)
        .def_readonly("transformed_y_best", &SingleObjectiveInfo::transformed_y_best)

        .def_readonly("y", &SingleObjectiveInfo::y)
        .def_readonly("y_best", &SingleObjectiveInfo::y_best)

        .def_readonly("x", &SingleObjectiveInfo::x)
        .def_readonly("violations", &SingleObjectiveInfo::violations)
        .def_readonly("penalties", &SingleObjectiveInfo::penalties)
        .def_readonly("objective", &SingleObjectiveInfo::optimum, "The best possible fitness value")
        .def_readonly("has_improved", &SingleObjectiveInfo::has_improved,
                      "Whether the last call to problem has caused global improvement.");

    py::class_<MultiObjectiveInfo>(m, "LogMultiInfo")
        .def(py::init<
                size_t,                                                    // evaluations
                std::vector<Solution<double, std::vector<double>>>,        // pareto_front
                std::vector<double>,                                       // y
                std::vector<double>,                                       // x
                std::vector<double>,                                       // violations
                std::vector<double>,                                       // penalties
                Solution<double, std::vector<double>>,                     // optimum
                bool                                                       // has_improved (default = false)
            >(),
            py::arg("evaluations"),
            py::arg("pareto_front"),
            py::arg("y"),
            py::arg("x"),
            py::arg("violations"),
            py::arg("penalties"),
            py::arg("optimum"),
            py::arg("has_improved") = false,
            R"pbdoc(
                LogInfo struct. Gets passed to the call method of a logger when it is invoked. 

                Parameters
                ----------
                evaluations: int
                    The current number of evaluations
                pareto_front: list<Solution>
                    The current pareto front
                y: np.npdarray | list
                    The transformed current fitness value with constraints applied
                x: np.npdarray | list
                    The current solution passed to problem
                violation: np.npdarray | list
                    The current solution constraint violations
                penalties: np.npdarray | list
                    The current solution constraint penalties
                optimum: list<solution>
                    The optimum solution for the problem
                has_improved: bool = True
                    Whether the evaluation has caused an improvement in the objective value

            )pbdoc")
        .def_readonly("evaluations", &MultiObjectiveInfo::evaluations,
                    "The number of function evaluations performed on the current problem so far")
        .def_readonly("pareto_front", &MultiObjectiveInfo::pareto_front,
                    "The current pareto front")
        .def_readonly("y", &MultiObjectiveInfo::y)
        .def_readonly("x", &MultiObjectiveInfo::x)
        .def_readonly("violations", &MultiObjectiveInfo::violations)
        .def_readonly("penalties", &MultiObjectiveInfo::penalties)
        .def_readonly("objective", &MultiObjectiveInfo::optimum, "The best possible fitness value")
        .def_readonly("has_improved", &MultiObjectiveInfo::has_improved,
                    "Whether the last call to problem has caused global improvement.");
}