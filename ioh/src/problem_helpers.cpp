#include "pch.hpp"

using namespace ioh::problem;

template <typename T, typename R>
void define_solution(nb::module_ &m, const std::string &name)
{
    using Class = Solution<T, R>;

    nb::class_<Class>(m, name.c_str()) 
        .def(nb::init<std::vector<T>, double>(), nb::arg("x"), nb::arg("y"),
             R"pbdoc(
            A Solution object, which is a container for a search point and its corresponding fitness value.

            Parameters
            ----------
            x: list
                the search point in a search space, e.g., R^d or {0,1}^d
            y: float
                the corresponding objective value of `x`, i.e., y = f(x)

        )pbdoc")
        .def_prop_rw(
            "x",
            [](Class &c) {
                return make_mutable_array(c.x, nb::cast(&c));
            },
            [](Class &self, const std::vector<T> &x) { self.x = x; },
            "The search point in a search space, e.g., R^d or {0,1}^d")
        .def_ro("y", &Class::y, "The corresponding objective value of `x`, i.e., y = f(x)")
        .def("__repr__", &Class::repr);
}

template <typename T, typename R>
void define_state(nb::module_ &m, const std::string &name)
{
    using Class = State<T, R>;
    using Class2 = Solution<T, R>;
    nb::class_<Class>(m, name.c_str())
        .def(nb::init<Class2>(), nb::arg("initial"),
             R"pbdoc(
                A state of a problem. Contains metrics such as the number of evaluations, and the current best value.

                Parameters
                ----------
                initial: Solution
                    an object to initialize the state, which contains a placeholder for a solution to an optimization problem. 
            )pbdoc")
        .def_ro("evaluations", &Class::evaluations, "The number of times the problem has been evaluated so far.")
        .def_ro("optimum_found", &Class::optimum_found,
                      "Boolean value indicating whether final_target_found optimum of a given problem has been found.")
        .def_ro("final_target_found", &Class::final_target_found,
                      "Boolean value indicating whether the final target of a given problem has been found.")
        .def_ro("current_best_internal", &Class::current_best_internal,
                      "The internal representation of the best so far solution. "
                      "See `current_internal` for a short explanation on the meaning of 'internal'")
        .def_ro("current_best", &Class::current_best, "The current best-so-far solution.")
        .def_ro("current_internal", &Class::current_internal,
                      "The internal representation of the last-evaluated solution. Note that, for a given "
                      "problem instance, the input search point will be transformed with an automorphism "
                      "and after evaluating the "
                      "transformed point, the resulting objective value will also be transformed with another "
                      "automorphism in the objective space. Such interal information are stored in this attribute.")
        .def_ro("current", &Class::current, "The last-evaluated solution.")
        .def_ro("y_unconstrained", &Class::y_unconstrained, "The current unconstrained value.")
        .def_ro("y_unconstrained_best", &Class::y_unconstrained_best, "The current best unconstrained value.")
        .def_ro("has_improved", &Class::has_improved,
                      "Whether the last call to problem has caused global improvement.")
        .def("__repr__", &Class::repr);
}


template <typename T>
struct PyConstraint : Constraint<T>
{
    NB_TRAMPOLINE(Constraint<T>, 1);

    bool compute_violation(const std::vector<T> &x) override
    {
        NB_OVERRIDE_PURE(compute_violation, make_array(x));
    }

    [[nodiscard]] std::string repr() const override { return "<AbstractConstraint>"; }
};

template <typename T>
void define_constraint(nb::module_ &m, const std::string &name)
{
    using Class = Constraint<T>;

    nb::class_<Class, PyConstraint<T>>(m, name.c_str())
        .def_rw("enforced", &Class::enforced, "The type of constraint enforcement applied.")
        .def_rw("weight", &Class::weight, "The weight given to this constraint")
        .def_rw("exponent", &Class::exponent, "The exponent for this constraint")
        .def("__call__", &Class::operator())
        .def("is_feasible", &Class::is_feasible)
        .def("compute_violation", &Class::compute_violation)
        .def("penalize", &Class::penalize)
        .def("violation", &Class::violation)
        .def("penalty", &Class::penalty);
}

template <typename T>
void define_constraintset(nb::module_ &m, const std::string &name)
{
    using Class = ConstraintSet<T>;

    nb::class_<Class>(m, name.c_str())
        .def(nb::init<Constraints<T>>(), nb::arg("constraints") = Constraints<T>{},
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
        .def("__repr__", &Class::repr)
        ;
}


bool register_python_fn(nb::handle f);

template <typename T>
void define_functionalconstraint(nb::module_ &m, const std::string &name)
{
    using Parent = Constraint<T>;
    using Class = FunctionalConstraint<T>;

    nb::class_<Class, Parent>(m, name.c_str())
        .def("__init__", 
                [](Class *t, nb::handle f, const double w, const double ex, const constraint::Enforced e, const std::string &n) {
                    register_python_fn(f);
                    auto fn = [f](const std::vector<T> &x) {
                    return nb::cast<double>(f(make_array(x)));
                    };
                    new (t) Class(fn, w, ex, e, n);
            },
             nb::arg("fn"), nb::arg("weight") = 1.0, nb::arg("exponent") = 1.0,
             nb::arg("enforced") = constraint::Enforced::SOFT, nb::arg("name") = "",
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
            )pbdoc"
        )
        .def("__repr__", &Class::repr);
}


template <typename T>
void define_bounds(nb::module_ &m, const std::string &name)
{

    using Parent = Constraint<T>;
    using Class = Bounds<T>;


    nb::class_<Class, Parent>(m, name.c_str())
        .def(nb::init<std::vector<T>, std::vector<T>, constraint::Enforced>(), nb::arg("lb"), nb::arg("ub"),
             nb::arg("enforced") = constraint::Enforced::NOT,
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
        .def(nb::init<int, T, T, constraint::Enforced>(), nb::arg("size"), nb::arg("lb"), nb::arg("ub"),
             nb::arg("enforced") = constraint::Enforced::NOT,
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
        .def_prop_rw(
            "ub",
            [](const Class &c) {
                return make_array(c.ub);
            },
            [](Class &c, const std::vector<T> &vec) { c.ub = vec; },
            "The upper bound (box constraint)")
        .def_prop_rw(
            "lb",
            [](const Class &c) {
                return make_array(c.lb);
            },
            [](Class &c, const std::vector<T> &vec) { c.lb = vec; },
            "The lower bound (box constraint)")
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
void define_constraint_types(nb::module_ &m, const std::string &name)
{
    define_constraint<T>(m, fmt::format("Abstract{}Constraint", name));
    define_constraintset<T>(m, fmt::format("{}ConstraintSet", name));
    define_functionalconstraint<T>(m, fmt::format("{}Constraint", name));
    define_bounds<T>(m, fmt::format("{}Bounds", name));
}


void define_helper_classes(nb::module_ &m)
{
    nb::enum_<ioh::common::OptimizationType>(
        m, "OptimizationType", "Enum used for defining whether the problem is subject to minimization or maximization")
        .value("MAX", ioh::common::OptimizationType::MAX)
        .value("MIN", ioh::common::OptimizationType::MIN)
        .export_values();

    nb::enum_<ioh::problem::constraint::Enforced>(m, "ConstraintEnforcement",
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
    define_constraint_types<int>(m, "Integer");
    define_constraint_types<double>(m, "Real");
    define_state<double, double>(m, "RealState");
    define_state<int, double>(m, "IntegerState");


    nb::class_<MetaData>(m, "MetaData")
        .def(nb::init<int, int, std::string, int, ioh::common::OptimizationType, double>(), nb::arg("problem_id"),
             nb::arg("instance"), nb::arg("name"), nb::arg("n_variables"), nb::arg("optimization_type"),
             nb::arg("final_target") = 1e-8,
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
                final_target: float
                    The final target of the function    
               
            )pbdoc")
        .def_ro("instance", &MetaData::instance, "The instance number of the current problem")
        .def_ro("problem_id", &MetaData::problem_id, "The id of the problem within its suite")
        .def_ro("name", &MetaData::name, "The name of the current problem")
        .def_prop_ro(
            "optimization_type", [](const MetaData &meta) { return meta.optimization_type.type(); },
            "The type of problem (maximization or minimization)")
        .def_ro("n_variables", &MetaData::n_variables,
                      "The number of variables (dimension) of the current problem")
        .def_ro("final_target", &MetaData::final_target)
        .def("__repr__", &MetaData::repr)
        .def("__eq__", &MetaData::operator==);

    nb::class_<ioh::logger::Info>(m, "LogInfo")
        .def(nb::init<size_t, double, double, double, double, double, double, std::vector<double>, std::vector<double>,
                      std::vector<double>, Solution<double, double>, bool>(),
             nb::arg("evaluations"), nb::arg("raw_y"), nb::arg("raw_y_best"), nb::arg("transformed_y"),
             nb::arg("transformed_y_best"), nb::arg("y"), nb::arg("y_best"), nb::arg("x"), nb::arg("violations"),
             nb::arg("penalties"), nb::arg("optimum"), nb::arg("has_improved") = false,
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
        .def_ro("evaluations", &ioh::logger::Info::evaluations,
                      "The number of function evaluations performed on the current problem so far")

        .def_ro("raw_y", &ioh::logger::Info::raw_y)
        .def_ro("raw_y_best", &ioh::logger::Info::raw_y_best)

        .def_ro("transformed_y", &ioh::logger::Info::transformed_y)
        .def_ro("transformed_y_best", &ioh::logger::Info::transformed_y_best)

        .def_ro("y", &ioh::logger::Info::y)
        .def_ro("y_best", &ioh::logger::Info::y_best)

        .def_ro("x", &ioh::logger::Info::x)
        .def_ro("violations", &ioh::logger::Info::violations)
        .def_ro("penalties", &ioh::logger::Info::penalties)
        .def_ro("objective", &ioh::logger::Info::optimum, "The best possible fitness value")
        .def_ro("has_improved", &ioh::logger::Info::has_improved,
                      "Whether the last call to problem has caused global improvement.");
}