#include "common.hpp"

#include <type_traits>

// problem.cpp: keeps a Python function alive until the end of the process

namespace ioh_python::problem_helpers
{
    //! The element type of the input arrays that a class over T takes without a conversion (numpy's default integer
    //! type for integers)
    template <typename T>
    using ArrayType = std::conditional_t<std::is_same_v<T, int>, int64_t, T>;

    template <typename T>
    using InputArray = Array1D<ArrayType<T>>;

    template <typename T, typename R>
    void define_solution(nb::module_ &m, const std::string &name)
    {
        using Class = ioh::problem::Solution<T, R>;

        nb::class_<Class>(m, name.c_str())
            .def(
                "__init__",
                [](Class *self, const InputArray<T> &x, double y) { new (self) Class(to_vector<T>(x), y); },
                array_arg<ArrayType<T>>("x"), nb::arg("y"))
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
                // a writable view of x, which keeps the Solution alive
                [](Class &c) { return make_mutable_array(c.x, nb::find(&c)); },
                [](Class &self, const std::vector<T> &x) { self.x = x; },
                "The search point in a search space, e.g., R^d or {0,1}^d")
            .def_ro("y", &Class::y, "The corresponding objective value of `x`, i.e., y = f(x)")
            .def("__repr__", &Class::repr);
    }

    template <typename T, typename R>
    void define_state(nb::module_ &m, const std::string &name)
    {
        using Class = ioh::problem::State<T, R>;
        using Class2 = ioh::problem::Solution<T, R>;
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

    //! The override of the method name in the Python object of the C++ object ptr of class Base: an attribute of
    //! the instance, or a method of a Python subclass. Like pybind11 (and unlike nanobind's trampolines, which look
    //! at the type only) this sees attributes of the instance too.
    template <typename Base>
    nb::object find_override(const Base *ptr, const char *name)
    {
        nb::object self = nb::find(ptr);
        if (!self.is_valid())
            return {};
        nb::object attributes = nb::getattr(self, "__dict__", nb::handle());
        if (!(attributes.is_valid() && nb::isinstance<nb::dict>(attributes) &&
              nb::borrow<nb::dict>(attributes).contains(name)))
        {
            // the method that the bindings define is no override
            nb::object method = nb::getattr(self.type(), name, nb::handle());
            if (!method.is_valid() || method.is(nb::getattr(nb::type<Base>(), name, nb::handle())))
                return {};
        }
        return nb::getattr(self, name);
    }

    //! The trampoline of the constraints
    template <typename T>
    struct PyConstraint : ioh::problem::Constraint<T>
    {
        using ioh::problem::Constraint<T>::Constraint;

        bool compute_violation(const std::vector<T> &x) override
        {
            nb::gil_scoped_acquire gil;
            nb::object f = find_override(static_cast<const ioh::problem::Constraint<T> *>(this), "compute_violation");
            if (!f.is_valid())
                throw std::runtime_error("Tried to call pure virtual function \"compute_violation\"");
            return nb::cast<bool>(f(make_array(x)));
        }

        [[nodiscard]] std::string repr() const override { return "<AbstractConstraint>"; }
    };

    template <typename T>
    void define_constraint(nb::module_ &m, const std::string &name)
    {
        using Class = ioh::problem::Constraint<T>;

        nb::class_<Class, PyConstraint<T>>(m, name.c_str())
            .def_rw("enforced", &Class::enforced, "The type of constraint enforcement applied.")
            .def_rw("weight", &Class::weight, "The weight given to this constraint")
            .def_rw("exponent", &Class::exponent, "The exponent for this constraint")
            .def(
                "__call__", [](Class &c, const InputArray<T> &x, const double y) { return c(to_vector<T>(x), y); },
                array_arg<ArrayType<T>>(), nb::arg())
            .def("__call__", &Class::operator())
            .def(
                "is_feasible", [](Class &c, const InputArray<T> &x) { return c.is_feasible(to_vector<T>(x)); },
                array_arg<ArrayType<T>>())
            .def("is_feasible", &Class::is_feasible)
            .def("compute_violation",
                 [](Class &c, const InputArray<T> &x) { return c.compute_violation(to_vector<T>(x)); },
                 array_arg<ArrayType<T>>())
            .def("compute_violation", &Class::compute_violation)
            .def("penalize", &Class::penalize)
            .def("penalty", &Class::penalty)
            .def("violation", &Class::violation);
    }

    template <typename T>
    void define_constraintset(nb::module_ &m, const std::string &name)
    {
        using Class = ioh::problem::ConstraintSet<T>;

        nb::class_<Class>(m, name.c_str())
            .def(nb::init<ioh::problem::Constraints<T>>(), nb::arg("constraints") = ioh::problem::Constraints<T>{},
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

    template <typename T>
    void define_functionalconstraint(nb::module_ &m, const std::string &name)
    {
        using Parent = ioh::problem::Constraint<T>;
        using Class = ioh::problem::FunctionalConstraint<T>;

        nb::class_<Class, Parent>(m, name.c_str())
            .def(
                "__init__",
                [](Class *self, nb::handle f, const double w, const double ex,
                   const ioh::problem::constraint::Enforced e, const std::string &n) {
                    // the constraint can end up in a registry (wrap_problem), which keeps it for the rest of the
                    // process
                    register_python_fn(f);
                    auto fn = [f](const std::vector<T> &x) {
                        nb::gil_scoped_acquire gil;
                        return nb::cast<double>(f(make_array(x)));
                    };
                    new (self) Class(fn, w, ex, e, n);
                },
                nb::arg("fn"), nb::arg("weight") = 1.0, nb::arg("exponent") = 1.0,
                nb::arg("enforced") = ioh::problem::constraint::Enforced::SOFT, nb::arg("name") = "",
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
    void define_bounds(nb::module_ &m, const std::string &name)
    {
        using Parent = ioh::problem::Constraint<T>;
        using Class = ioh::problem::Bounds<T>;
        using ioh::problem::constraint::Enforced;

        nb::class_<Class, Parent>(m, name.c_str())
            .def(
                "__init__",
                [](Class *self, const InputArray<T> &lb, const InputArray<T> &ub, const Enforced enforced) {
                    new (self) Class(to_vector<T>(lb), to_vector<T>(ub), enforced);
                },
                array_arg<ArrayType<T>>("lb"), array_arg<ArrayType<T>>("ub"), nb::arg("enforced") = Enforced::NOT)
            .def(nb::init<std::vector<T>, std::vector<T>, Enforced>(), nb::arg("lb"), nb::arg("ub"),
                 nb::arg("enforced") = Enforced::NOT,
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
            .def(nb::init<int, T, T, Enforced>(), nb::arg("size"), nb::arg("lb"), nb::arg("ub"),
                 nb::arg("enforced") = Enforced::NOT,
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
            )pbdoc")
            // copies on read; the arrays own their data, so the getters do not tie them to the bounds
            .def_prop_rw(
                "ub", [](const Class &c) { return make_array(c.ub); },
                [](Class &c, const std::vector<T> &vec) { c.ub = vec; }, nb::for_getter(nb::rv_policy::reference),
                "The upper bound (box constraint)")
            .def_prop_rw(
                "lb", [](const Class &c) { return make_array(c.lb); },
                [](Class &c, const std::vector<T> &vec) { c.lb = vec; }, nb::for_getter(nb::rv_policy::reference),
                "The lower bound (box constraint)")
            .def("__repr__", &Class::repr)
            .def("compute_violation",
                 [](Class &c, const InputArray<T> &x) { return c.compute_violation(to_vector<T>(x)); },
                 array_arg<ArrayType<T>>())
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

    //! int(e) and operator.index(e) of an enum, as pybind11's enums have them (nanobind's are enum.Enum)
    template <typename E>
    void define_int_conversion(nb::enum_<E> &e)
    {
        e.def("__int__", [](const E v) { return static_cast<int64_t>(v); })
            .def("__index__", [](const E v) { return static_cast<int64_t>(v); });
    }
} // namespace ioh_python::problem_helpers

void define_helper_classes(nb::module_ &m)
{
    using namespace ioh_python::problem_helpers;
    using ioh::problem::MetaData;

    nb::enum_<ioh::common::OptimizationType> optimization_type(
        m, "OptimizationType", "Enum used for defining whether the problem is subject to minimization or maximization");
    optimization_type.value("MAX", ioh::common::OptimizationType::MAX)
        .value("MIN", ioh::common::OptimizationType::MIN)
        .export_values();
    define_int_conversion(optimization_type);

    nb::enum_<ioh::problem::constraint::Enforced> enforced(m, "ConstraintEnforcement",
                                                          "Enum defining constraint handling strategies.");
    enforced.value("NOT", ioh::problem::constraint::Enforced::NOT, "Do not calculate the constraint at all")
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
    define_int_conversion(enforced);

    define_solution<double, double>(m, "RealSolution");
    define_solution<int, double>(m, "IntegerSolution");
    define_constraint_types<int>(m, "Integer");
    define_constraint_types<double>(m, "Real");
    define_state<double, double>(m, "RealState");
    define_state<int, double>(m, "IntegerState");

    nb::class_<MetaData> meta_data(m, "MetaData");
    meta_data
        .def(nb::init<int, int, std::string, int, ioh::common::OptimizationType, double, bool, bool>(),
             nb::arg("problem_id"), nb::arg("instance"), nb::arg("name"), nb::arg("n_variables"),
             nb::arg("optimization_type"), nb::arg("final_target") = 1e-8, nb::arg("fixed_dimension") = false,
             nb::arg("fixed_instance") = false,
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
                fixed_dimension: bool
                    Whether the problem has this dimension whatever dimension was requested
                fixed_instance: bool
                    Whether the problem has this instance whatever instance was requested

            )pbdoc")
        .def_ro("instance", &MetaData::instance, "The instance number of the current problem")
        .def_ro("problem_id", &MetaData::problem_id, "The id of the problem within its suite")
        .def_ro("name", &MetaData::name, "The name of the current problem")
        .def_prop_ro(
            "optimization_type", [](const MetaData &meta) { return meta.optimization_type.type(); },
            "The type of problem (maximization or minimization)")
        .def_ro("n_variables", &MetaData::n_variables, "The number of variables (dimension) of the current problem")
        .def_ro("final_target", &MetaData::final_target)
        .def_ro("fixed_dimension", &MetaData::fixed_dimension,
                "Whether the problem has one dimension, n_variables, whatever dimension was requested (the "
                "CEC2013 and graph problems)")
        .def_ro("fixed_instance", &MetaData::fixed_instance,
                "Whether the problem has one instance, instance, whatever instance was requested (the CEC2013 "
                "and graph problems)")
        .def("__repr__", &MetaData::repr)
        .def("__eq__", &MetaData::operator==);
    // as with pybind11, which sets __hash__ to None for a class that defines __eq__ but not __hash__
    meta_data.attr("__hash__") = nb::none();

    using Info = ioh::logger::Info;
    nb::class_<Info>(m, "LogInfo")
        .def(nb::init<size_t, double, double, double, double, double, double, std::vector<double>, std::vector<double>,
                      std::vector<double>, ioh::problem::Solution<double, double>, bool>(),
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
        .def_ro("evaluations", &Info::evaluations,
                "The number of function evaluations performed on the current problem so far")

        .def_ro("raw_y", &Info::raw_y)
        .def_ro("raw_y_best", &Info::raw_y_best)

        .def_ro("transformed_y", &Info::transformed_y)
        .def_ro("transformed_y_best", &Info::transformed_y_best)

        .def_ro("y", &Info::y)
        .def_ro("y_best", &Info::y_best)

        .def_ro("x", &Info::x)
        .def_ro("violations", &Info::violations)
        .def_ro("penalties", &Info::penalties)
        .def_ro("objective", &Info::optimum, "The best possible fitness value")
        .def_ro("has_improved", &Info::has_improved, "Whether the last call to problem has caused global improvement.");
}
