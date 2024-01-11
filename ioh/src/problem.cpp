#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "ioh.hpp"

#include <iostream>

namespace py = pybind11;
using namespace ioh::problem;

static std::vector<py::handle> WRAPPED_FUNCTIONS;

bool register_python_fn(py::handle f)
{
    f.inc_ref();
    WRAPPED_FUNCTIONS.push_back(f);
    return true;
}


template <typename T, typename R>
void define_solution(py::module &m, const std::string &name)
{
    using Class = Solution<T, R>;

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
        .def_property(
            "x", [](const Class &c) { return py::array(c.x.size(), c.x.data()); },
            [](Class &self, const std::vector<T> &x) { self.x = x; },
            "The search point in a search space, e.g., R^d or {0,1}^d")
        .def_readonly("y", &Class::y, "The corresponding objective value of `x`, i.e., y = f(x)")
        .def("__repr__", &Class::repr);
}

template <typename T, typename R>
void define_state(py::module &m, const std::string &name)
{
    using Class = State<T, R>;
    using Class2 = Solution<T, R>;
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
        .def_readonly("y_unconstrained", &Class::y_unconstrained, "The current unconstrained value.")
        .def_readonly("y_unconstrained_best", &Class::y_unconstrained_best, "The current best unconstrained value.")
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
             py::arg("enforced") = constraint::Enforced::SOFT,
             py::arg("name") = "",
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
                  [](Class &c, const std::vector<T>& vec) { c.ub = vec;},
            "The upper bound (box constraint)")
        .def_property(
            "lb", [](const Class &c) { return py::array(c.lb.size(), c.lb.data()); },
                  [](Class &c, const std::vector<T>& vec) { c.lb = vec;},
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
void define_constraint_types(py::module &m, const std::string &name)
{
    define_constraint<T>(m, fmt::format("Abstract{}Constraint", name));
    define_constraintset<T>(m, fmt::format("{}ConstraintSet", name));
    define_functionalconstraint<T>(m, fmt::format("{}Constraint", name));
    define_bounds<T>(m, fmt::format("{}Bounds", name));
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
        auto bounds = this->bounds();
        auto constraints = this->constraints();
        auto optimum = this->optimum();

        std::cerr << "PyProblem meta_data.name: " << meta_data.name << std::endl;

        auto &factory = ioh::common::Factory<P, int, int>::instance();
        factory.include(meta_data.name, meta_data.problem_id, [=](const int instance, const int n_variables) {
            return std::make_shared<PyProblem<P, T>>(MetaData(meta_data.problem_id, instance, meta_data.name,
                                                              n_variables, meta_data.optimization_type.type()),
                                                     bounds.resize(n_variables), constraints, optimum);
        });
        return true;
    }

    static Solution<T, SingleObjective> validate_optimum(const Solution<T, SingleObjective> &opt, const int n_variables,
                                                         const bool is_minimization)
    {
        if (opt.x.size() == static_cast<size_t>(n_variables))
            return opt;
        return Solution<T, SingleObjective>(n_variables, ioh::common::OptimizationType(is_minimization));
    }

public:
    explicit PyProblem(const MetaData &meta_data, const Bounds<T> &bounds, const ConstraintSet<T> &constraints,
                       const Solution<T, SingleObjective> &opt) :
        P(meta_data, bounds, constraints, opt)
    {
    }

    explicit PyProblem(const std::string &name, const int n_variables = 5, const int instance = 1,
                       const bool is_minimization = true, Bounds<T> bounds = Bounds<T>(),
                       const Constraints<T> &constraints = {}, const Solution<T, SingleObjective> &optimum = {}) :
        P(MetaData(instance, name, n_variables,
                   is_minimization ? ioh::common::OptimizationType::MIN : ioh::common::OptimizationType::MAX),
          bounds, ConstraintSet<T>(constraints), validate_optimum(optimum, n_variables, is_minimization))
    {
        auto registered = perform_registration();
    }

    double evaluate(const std::vector<T> &x) override
    {
        PYBIND11_OVERRIDE_PURE(double, P, evaluate, py::array(x.size(), x.data()));
    }

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
        .def(py::init<const std::string, int, int, bool, Bounds<T>, Constraints<T>, Solution<T, SingleObjective>>(),
             py::arg("name"), py::arg("n_variables") = 5, py::arg("instance") = 1, py::arg("is_minimization") = true,
             py::arg("bounds") = Bounds<T>(5), py::arg("constraints") = Constraints<T>{},
             py::arg("optimum") = Solution<T, SingleObjective>{},
             fmt::format("Base class for {} problems", name).c_str())
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
        .def("__call__", py::overload_cast<const std::vector<T>&>(&ProblemType::operator()),
             R"pbdoc(
                Evaluate the problem.

                Parameters
                ----------
                    x: list
                        the search point to evaluate. It must be a 1-dimensional array/list whose length matches search space's dimensionality
                Returns
                -------
                float
                    The evaluated search point
            )pbdoc")
        .def("__call__", py::overload_cast<const std::vector<std::vector<T>>&>(&ProblemType::operator()),
             R"pbdoc(
                Evaluate the problem.

                Parameters
                ----------
                    x: list[list]
                        the search points to evaluate. It must be a 2-dimensional array/list whose length matches search space's dimensionality
                Returns
                -------
                list[float]
                    The evaluated search points
            )pbdoc")
        .def_static(
            "create",
            [](const std::string &name, int iid, int dim) { return Factory::instance().create(name, iid, dim); },
            py::arg("problem_name"), py::arg("instance_id"), py::arg("dimension"),
            R"pbdoc(
                Create a problem instance

                Parameters
                ----------
                    problem_name: a string indicating the problem name. 
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
                    problem_id: the index of the problem to create. 
                    instance_id: an integer identifier of the problem instance, which seeds the random generation
                        of the tranformations in the search/objective spaces
                    dimension: integer, representing the dimensionality of the search space
            )pbdoc")
        .def_property_readonly_static(
            "problems", [](py::object) { return Factory::instance().map(); }, "All registered problems")
        .def_property("log_info", &ProblemType::log_info, &ProblemType::set_log_info,
                      "The data is that being sent to the logger.")
        .def_property_readonly(
            "state", &ProblemType::state,
            "The current state of the optimization process containing, e.g., the current solution and the "
            "number of function evaluated consumed so far")
        .def_property_readonly("meta_data", &ProblemType::meta_data,
                               "The static meta-data of the problem containing, e.g., problem id, instance id, and "
                               "problem's dimensionality")
        .def_property_readonly("optimum", &ProblemType::optimum,
                               "The optimum and its objective value for a problem instance")
        .def_property_readonly("bounds", &ProblemType::bounds, "The bounds of the problem.")
        .def_property_readonly("constraints", &ProblemType::constraints, "The constraints of the problem.")
        .def("add_constraint", &ProblemType::add_constraint, "add a constraint")
        .def("remove_constraint", &ProblemType::remove_constraint, "remove a constraint")
        .def("enforce_bounds", &ProblemType::enforce_bounds, py::arg("weight") = 1.,
             py::arg("how") = constraint::Enforced::SOFT, py::arg("exponent") = 1., 
            R"pbdoc(
                Enforced the bounds (box-constraints) as constraint
                Parameters
                ----------
                    weight: The weight for computing the penalty (can be infinity to have strict box-constraints)
                    how: The enforcement strategy, should be one of the 'ioh.ConstraintEnforcement' options
                    exponent: The exponent for scaling the contraint
                )pbdoc")

        .def("set_id", &ProblemType::set_id, py::arg("new_problem_id"), "update the problem id")
        .def("set_instance", &ProblemType::set_instance, py::arg("new_instance"), "update the problem instance")
        .def("set_name", &ProblemType::set_name, py::arg("new_name"),"update the problem name")
        .def("__repr__", [=](const ProblemType &p) {
            using namespace ioh::common;
            const auto meta_data = p.meta_data();
            return "<" + name + fmt::format("Problem {:d}. ", meta_data.problem_id) + meta_data.name +
                fmt::format(" (iid={:d} dim={:d})>", meta_data.instance, meta_data.n_variables);
        });
}


template <typename T>
void define_wrapper_functions(py::module &m, const std::string &class_name, const std::string &function_name)
{
    using WrappedProblem = SingleObjectiveWrappedProblem<T>;
    py::class_<WrappedProblem, SingleObjectiveProblem<T>, std::shared_ptr<WrappedProblem>>(m, class_name.c_str(),
                                                                                           py::buffer_protocol());

    m.def(
        function_name.c_str(),
        [](py::handle f, const std::string &name, ioh::common::OptimizationType t, std::optional<double> lb,
           std::optional<double> ub, std::optional<py::handle> tx, std::optional<py::handle> ty,
           std::optional<py::handle> co, Constraints<T> cs) {
            register_python_fn(f);
            auto of = [f](const std::vector<T> &x) { 
                return PyFloat_AsDouble(f(py::array(x.size(), x.data())).ptr()); 
            };

            auto ptx = [tx](std::vector<T> x, const int iid) {
                if (tx)
                {
                    static bool r = register_python_fn(tx.value());
                    py::list px = (tx.value()(py::array(x.size(), x.data()), iid));
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
                        return Solution<T, double>(x.cast<std::vector<T>>(), y.cast<double>());
                    }
                    return xt.cast<Solution<T, double>>();
                }
                return Solution<T, double>(dim, t);
            };

            wrap_function<T, double>(of, name, t, lb, ub, ptx, pty, pco, cs);
        },
        py::arg("f"), py::arg("name"), py::arg("optimization_type") = ioh::common::OptimizationType::MIN,
        py::arg("lb") = std::nullopt, py::arg("ub") = std::nullopt, py::arg("transform_variables") = std::nullopt,
        py::arg("transform_objectives") = std::nullopt, py::arg("calculate_objective") = std::nullopt,
        py::arg("constraints") = Constraints<T>()

    );
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

void define_helper_classes(py::module &m)
{
    py::enum_<ioh::common::OptimizationType>(
        m, "OptimizationType", "Enum used for defining whether the problem is subject to minimization or maximization")
        .value("MAX", ioh::common::OptimizationType::MAX)
        .value("MIN", ioh::common::OptimizationType::MIN)
        .export_values();

    py::enum_<ioh::problem::constraint::Enforced>(m, "ConstraintEnforcement",
                                                  "Enum defining constraint handling strategies.")
        .value("NOT", ioh::problem::constraint::Enforced::NOT, 
                "Do not calculate the constraint at all")
        .value("HIDDEN", ioh::problem::constraint::Enforced::HIDDEN, 
                "Do not enforce, but still calculate to enable logging.")
        .value("SOFT", ioh::problem::constraint::Enforced::SOFT,
               "Penalize (y + p), but aggregate all the constraint penalties into a sum ")
        .value("HARD", ioh::problem::constraint::Enforced::HARD,
               "Penalize (p only), and if violation return only the penalty for this constraint in contraintset")
        .value("OVERRIDE", ioh::problem::constraint::Enforced::OVERRIDE,
               "Penalize (p only), and if violation return the custom penalization function this constraint in contraintset")
        .export_values();

    define_solution<double, double>(m, "RealSolution");
    define_solution<int, double>(m, "IntegerSolution");
    define_constraint_types<int>(m, "Integer");
    define_constraint_types<double>(m, "Real");
    define_state<double, double>(m, "RealState");
    define_state<int, double>(m, "IntegerState");


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
        .def_property_readonly(
            "optimization_type", [](const MetaData &meta) { return meta.optimization_type.type(); },
            "The type of problem (maximization or minimization)")
        .def_readonly("n_variables", &MetaData::n_variables,
                      "The number of variables (dimension) of the current problem")
        .def("__repr__", &MetaData::repr)
        .def("__eq__", &MetaData::operator==);

    py::class_<ioh::logger::Info>(m, "LogInfo")
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
        .def_readonly("evaluations", &ioh::logger::Info::evaluations,
                      "The number of function evaluations performed on the current problem so far")

        .def_readonly("raw_y", &ioh::logger::Info::raw_y)
        .def_readonly("raw_y_best", &ioh::logger::Info::raw_y_best)

        .def_readonly("transformed_y", &ioh::logger::Info::transformed_y)
        .def_readonly("transformed_y_best", &ioh::logger::Info::transformed_y_best)

        .def_readonly("y", &ioh::logger::Info::y)
        .def_readonly("y_best", &ioh::logger::Info::y_best)

        .def_readonly("x", &ioh::logger::Info::x)
        .def_readonly("violations", &ioh::logger::Info::violations)
        .def_readonly("penalties", &ioh::logger::Info::penalties)
        .def_readonly("objective", &ioh::logger::Info::optimum, "The best possible fitness value")
        .def_readonly("has_improved", &ioh::logger::Info::has_improved,
                      "Whether the last call to problem has caused global improvement.");
}

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

void define_cec_problems(py::module &m)
{
    py::class_<CEC, RealSingleObjective, std::shared_ptr<CEC>>(
        m,
        "CEC",
        R"pbdoc(
            Functions from the CEC 2022 conference.
        )pbdoc"
    )
    .def_static(
        "create",
        [](const std::string &name, int iid, int dim) {
            return ioh::common::Factory<CEC, int, int>::instance().create(name, iid, dim);
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
            return ioh::common::Factory<CEC, int, int>::instance().create(id, iid, dim);
        },
        py::arg("problem_id"), py::arg("instance_id"), py::arg("dimension"),
        R"pbdoc(
            Create a problem instance

            Parameters
            ----------
                problem_id: int
                    a number indicating the problem numeric identifier.
                instance_id: int
                    an integer identifier of the problem instance
                dimension: int
                    the dimensionality of the search space
        )pbdoc")
    .def_property_readonly_static(
        "problems", [](py::object) { return ioh::common::Factory<CEC, int, int>::instance().map(); },
        "All registered problems");

    py::class_<cec::CEC_Zakharov, CEC, std::shared_ptr<cec::CEC_Zakharov>>(m, "CEC_Zakharov", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec::CEC_Rosenbrock, CEC, std::shared_ptr<cec::CEC_Rosenbrock>>(m, "CEC_Rosenbrock", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec::CEC_ExpandedSchafferF7, CEC, std::shared_ptr<cec::CEC_ExpandedSchafferF7>>(m, "CEC_ExpandedSchafferF7", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec::CEC_Rastrigin, CEC, std::shared_ptr<cec::CEC_Rastrigin>>(m, "CEC_Rastrigin", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec::CEC_Levy, CEC, std::shared_ptr<cec::CEC_Levy>>(m, "CEC_Levy", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec::CEC_HybridFunction1, CEC, std::shared_ptr<cec::CEC_HybridFunction1>>(m, "CEC_HybridFunction1", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec::CEC_HybridFunction2, CEC, std::shared_ptr<cec::CEC_HybridFunction2>>(m, "CEC_HybridFunction2", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec::CEC_HybridFunction3, CEC, std::shared_ptr<cec::CEC_HybridFunction3>>(m, "CEC_HybridFunction3", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec::CEC_CompositionFunction1, CEC, std::shared_ptr<cec::CEC_CompositionFunction1>>(m, "CompositionFunction1", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec::CEC_CompositionFunction2, CEC, std::shared_ptr<cec::CEC_CompositionFunction2>>(m, "CompositionFunction2", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec::CEC_CompositionFunction3, CEC, std::shared_ptr<cec::CEC_CompositionFunction3>>(m, "CompositionFunction3", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec::CEC_CompositionFunction4, CEC, std::shared_ptr<cec::CEC_CompositionFunction4>>(m, "CompositionFunction4", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
}

std::string to_lower(const std::string &s)
{
    auto res = s;
    std::transform(res.begin(), res.end(), res.begin(), [](unsigned char c) { return std::tolower(c); });
    return res;
}

template <typename P>
void define_bbob_problems(py::module &mi, const std::string &name = "BBOB", const bool submodule = false)
{
    py::class_<P, RealSingleObjective, std::shared_ptr<P>>(mi, name.c_str(),
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
            [](const std::string &name, int iid, int dim) {
                return ioh::common::Factory<P, int, int>::instance().create(name, iid, dim);
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
            [](int id, int iid, int dim) { return ioh::common::Factory<P, int, int>::instance().create(id, iid, dim); },
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
            "problems", [](py::object) { return ioh::common::Factory<P, int, int>::instance().map(); },
            "All registered problems");

    auto m = mi;
    if (submodule)
        m = mi.def_submodule(to_lower(name).c_str(), fmt::format("module containing {} problems", name).c_str());

    py::class_<bbob::Sphere<P>, P, std::shared_ptr<bbob::Sphere<P>>>(m, "Sphere", py::is_final(), "Sphere function")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Ellipsoid<P>, P, std::shared_ptr<bbob::Ellipsoid<P>>>(m, "Ellipsoid", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Rastrigin<P>, P, std::shared_ptr<bbob::Rastrigin<P>>>(m, "Rastrigin", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::BuecheRastrigin<P>, P, std::shared_ptr<bbob::BuecheRastrigin<P>>>(m, "BuecheRastrigin",
                                                                                       py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::LinearSlope<P>, P, std::shared_ptr<bbob::LinearSlope<P>>>(m, "LinearSlope", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::AttractiveSector<P>, P, std::shared_ptr<bbob::AttractiveSector<P>>>(m, "AttractiveSector",
                                                                                         py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::StepEllipsoid<P>, P, std::shared_ptr<bbob::StepEllipsoid<P>>>(m, "StepEllipsoid", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Rosenbrock<P>, P, std::shared_ptr<bbob::Rosenbrock<P>>>(m, "Rosenbrock", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::RosenbrockRotated<P>, P, std::shared_ptr<bbob::RosenbrockRotated<P>>>(m, "RosenbrockRotated",
                                                                                           py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::EllipsoidRotated<P>, P, std::shared_ptr<bbob::EllipsoidRotated<P>>>(m, "EllipsoidRotated",
                                                                                         py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Discus<P>, P, std::shared_ptr<bbob::Discus<P>>>(m, "Discus", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::BentCigar<P>, P, std::shared_ptr<bbob::BentCigar<P>>>(m, "BentCigar", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::SharpRidge<P>, P, std::shared_ptr<bbob::SharpRidge<P>>>(m, "SharpRidge", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::DifferentPowers<P>, P, std::shared_ptr<bbob::DifferentPowers<P>>>(m, "DifferentPowers",
                                                                                       py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::RastriginRotated<P>, P, std::shared_ptr<bbob::RastriginRotated<P>>>(m, "RastriginRotated",
                                                                                         py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Weierstrass<P>, P, std::shared_ptr<bbob::Weierstrass<P>>>(m, "Weierstrass", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Schaffers10<P>, P, std::shared_ptr<bbob::Schaffers10<P>>>(m, "Schaffers10", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Schaffers1000<P>, P, std::shared_ptr<bbob::Schaffers1000<P>>>(m, "Schaffers1000", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::GriewankRosenbrock<P>, P, std::shared_ptr<bbob::GriewankRosenbrock<P>>>(m, "GriewankRosenbrock",
                                                                                             py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Schwefel<P>, P, std::shared_ptr<bbob::Schwefel<P>>>(m, "Schwefel", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Gallagher101<P>, P, std::shared_ptr<bbob::Gallagher101<P>>>(m, "Gallagher101", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Gallagher21<P>, P, std::shared_ptr<bbob::Gallagher21<P>>>(m, "Gallagher21", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Katsuura<P>, P, std::shared_ptr<bbob::Katsuura<P>>>(m, "Katsuura", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::LunacekBiRastrigin<P>, P, std::shared_ptr<bbob::LunacekBiRastrigin<P>>>(m, "LunacekBiRastrigin",
                                                                                             py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
}

void define_problem_bases(py::module &m)
{
    define_base_class<RealSingleObjective, double>(m, "RealSingleObjective");
    define_base_class<IntegerSingleObjective, int>(m, "IntegerSingleObjective");
    define_wrapper_functions<double>(m, "RealSingleObjectiveWrappedProblem", "wrap_real_problem");
    define_wrapper_functions<int>(m, "IntegerSingleObjectiveWrappedProblem", "wrap_integer_problem");
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

        )pbdoc" 
        )
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

enum class SamplerType
{
    UNIFORM,
    SOBOL,
    HALTON
};

template <typename T>
T star_discrepancy_init(const int instance, const int n_variables, const int n_samples, const SamplerType sampler_type)
{
    using namespace ioh::common::random::sampler;

    std::vector<std::vector<double>> grid;
    switch (sampler_type)
    {
    case SamplerType::UNIFORM:
        grid = star_discrepancy::generate_grid<Uniform<double>>(instance, n_variables, n_samples);
        break;
    case SamplerType::HALTON:
        grid = star_discrepancy::generate_grid<Halton>(instance, n_variables, n_samples);
        break;
    default:
    case SamplerType::SOBOL:
        grid = star_discrepancy::generate_grid<Sobol>(instance, n_variables, n_samples);
    }
    const std::string py_name =
        std::is_base_of_v<ioh::problem::RealSingleObjective, T> ? "RealStarDiscrepancy" : "IntegerStarDiscrepancy";
    return T(0, instance, n_variables, py_name, grid);
}


template <typename T, typename P>
void define_star_discrepancy_problem(py::module &m, const std::string &name)
{
    py::class_<T, P, std::shared_ptr<T>>(m, name.c_str(), R"pbdoc(
            Star-discrepancy problems
             
            The 𝐿_infinity star discrepancy is a measure for the regularity of a finite set
            of points taken from [0, 1)^d. Calculating this measure is challenging, and exact algorithm
            fall short even for relatively small grids. This suite provides access to a set of these problems
            to tackle the problem from an optimization perspective.

            There are two equivalent representations for these problems: Integer-based and Real-valued. 

            For the Real-valued, the goal is to find the a point in [0,1)^d which minimizes the resulting discepancy directly.

            The Integer-valued version takes advantage of the fact that for each dimension, the optimium 
            has a value matching an existing point int the grid. As such, the search for a point in the grid can 
            be reduced to finding in each dimension the index of the coordinate, resulting in a search domain 
            [0,n]^d, where n is the number of gridpoints. 


            Reference
            ---------
            [Clement23] Clement, Francois, Diederick Vermetten, Jacob de Nobel, Alexandre Jesus, 
            Luís Paquete, and Carola Doerr. "Computing Star Discrepancies with 
            Numerical Black-Box Optimization Algorithms."

        )pbdoc")
        .def(py::init(&star_discrepancy_init<T>), py::arg("instance") = 1, py::arg("n_variables") = 5,
             py::arg("n_samples") = 5, py::arg("sampler_type") = SamplerType::UNIFORM,
             R"pbdoc(
                Star Discrepancy Problems

                Parameters
                ----------
                instance: int = 1
                    The instance of the problem, will be used as seed for sampler
                n_variables: int = 5
                    The dimension of the problem
                n_samples: int = 5
                    The number of sampled points
                sampler_type: StarDiscrepancySampler
                    The type of sampler to use when sampling the points
             )pbdoc")
        .def_property_readonly("grid",
                               [](const T &self) {
                                   const auto grid = self.get_grid();
                                   const auto n = grid.size(), m = grid[0].size();

                                   py::array_t<double, py::array::c_style> arr({n, m});

                                   auto ra = arr.mutable_unchecked();

                                   for (size_t i = 0; i < n; i++)
                                       for (size_t j = 0; j < m; j++)
                                           ra(i, j) = grid[i][j];

                                   return arr;
                               })
        .def_static(
            "create",
            [](const std::string &name, int iid, int dim) {
                return ioh::common::Factory<T, int, int>::instance().create(name, iid, dim);
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
            [](int id, int iid, int dim) { return ioh::common::Factory<T, int, int>::instance().create(id, iid, dim); },
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
            "problems", [](py::object) { return ioh::common::Factory<T, int, int>::instance().map(); },
            "All registered problems");
    ;
}


void define_star_discrepancy_problems(py::module &m)
{
    using namespace ioh::problem::star_discrepancy;
    using namespace ioh::common::random::sampler;

    py::enum_<SamplerType>(m, "StarDiscrepancySampler", 
        "Methods which can be used to sample the initial grids for the star discrepancy problems")
        .value("UNIFORM", SamplerType::UNIFORM)
        .value("HALTON", SamplerType::HALTON)
        .value("SOBOL", SamplerType::SOBOL)
        .export_values();

    define_star_discrepancy_problem<real::StarDiscrepancy, RealSingleObjective>(m, "RealStarDiscrepancy");
    define_star_discrepancy_problem<integer::StarDiscrepancy, IntegerSingleObjective>(m, "IntegerStarDiscrepancy");
}

void define_problem(py::module &m)
{
    define_problem_bases(m);
    define_bbob_problems<ioh::problem::BBOB>(m);
    define_bbob_problems<ioh::problem::SBOX>(m, "SBOX", true);
    define_pbo_problems(m);
    define_cec_problems(m);
    define_wmodels(m);
    define_submodular_problems(m);
    define_star_discrepancy_problems(m);
    
    py::class_<ioh::problem::bbob::ManyAffine, ioh::problem::RealSingleObjective, std::shared_ptr<ioh::problem::bbob::ManyAffine>>(m, "ManyAffine")
            .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"))
            .def(
                py::init<
                    std::vector<double>, 
                    std::array<double, 24>, 
                    std::array<int, 24>, 
                    int,
                    std::array<double, 24>>(),
                py::arg("xopt"),
                py::arg("weights"),
                py::arg("instances"),
                py::arg("n_variables"),
                py::arg("scale_factors") = ioh::problem::bbob::ManyAffine::default_scales)
            .def_property_readonly("weights", &ioh::problem::bbob::ManyAffine::get_weights)
            .def_property_readonly("instances", &ioh::problem::bbob::ManyAffine::get_instances)
            .def_property_readonly("scale_factors", &ioh::problem::bbob::ManyAffine::get_scale_factors)
            .def_property_readonly("sub_problems", &ioh::problem::bbob::ManyAffine::get_problems)
            .def_property_readonly("function_values", &ioh::problem::bbob::ManyAffine::get_function_values)
            ;

    py::module_::import("atexit").attr("register")(py::cpp_function([]() {
        // std::cout << "exiting gracefully...";
        for (const auto fn : WRAPPED_FUNCTIONS)
            if (fn)
                fn.dec_ref();
        // std::cout << " done\n";
    }));
}
