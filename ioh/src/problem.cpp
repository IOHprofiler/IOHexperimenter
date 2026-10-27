#include "common.hpp"

#include <mutex>
#include <type_traits>

namespace ioh_python::problem
{
    //! The Python functions that the registries hold (wrap_problem's objectives and transformations, the functions
    //! of RealConstraint/IntegerConstraint). The registries live until the end of the process, so they keep plain
    //! handles, and the references are released at exit, while the interpreter is still alive.
    std::vector<nb::handle> wrapped_functions;

    //! Guards wrapped_functions: Python functions may be registered from different threads at once
    std::mutex wrapped_functions_mutex;
} // namespace ioh_python::problem

//! Keeps f alive until the end of the process (also used by problem_helpers.cpp)
bool register_python_fn(nb::handle f)
{
    f.inc_ref();
    const std::lock_guard<std::mutex> lock(ioh_python::problem::wrapped_functions_mutex);
    ioh_python::problem::wrapped_functions.push_back(f);
    return true;
}

namespace ioh_python::problem
{
    //! The method of a Python subclass that overrides a virtual method: the attribute name of the Python object of
    //! ptr, if there is one. Like pybind11 (and unlike nanobind's trampolines) this finds attributes of the instance
    //! too, and a problem that C++ created (e.g. the registry, for a registered subclass) has no override.
    template <typename Base>
    nb::object find_override(const Base *ptr, const char *name)
    {
        nb::object self = nb::find(ptr);
        if (!self.is_valid())
            return {};
        return nb::getattr(self, name, nb::handle());
    }

    //! The trampoline of the problem base classes, which Python subclasses
    template <typename P, typename T>
    class PyProblem : public P
    {
        [[nodiscard]] bool perform_registration()
        {
            // like wrap_function: the id of the name if it is registered, otherwise the first free id, chosen under
            // the factory's lock
            auto &factory = ioh::common::Factory<P, int, int>::instance();
            auto meta_data = this->meta_data();
            auto bounds = this->bounds();
            auto constraints = this->constraints();
            auto optimum = this->optimum();
            this->set_id(factory.include_with_id(meta_data.name, 1, [&](const int id) {
                return [=](const int instance, const int n_variables) {
                    return std::make_shared<PyProblem<P, T>>(
                        ioh::problem::MetaData(id, instance, meta_data.name, n_variables,
                                               meta_data.optimization_type.type()),
                        bounds.resize(n_variables), constraints, optimum);
                };
            }));
            return true;
        }

        static ioh::problem::Solution<T, ioh::problem::SingleObjective>
        validate_optimum(const ioh::problem::Solution<T, ioh::problem::SingleObjective> &opt, const int n_variables,
                         const bool is_minimization)
        {
            if (opt.x.size() == static_cast<size_t>(n_variables))
                return opt;
            return ioh::problem::Solution<T, ioh::problem::SingleObjective>(
                n_variables, ioh::common::OptimizationType(is_minimization));
        }

        nb::object override_of(const char *name) const { return find_override(static_cast<const P *>(this), name); }

    public:
        explicit PyProblem(const ioh::problem::MetaData &meta_data, const ioh::problem::Bounds<T> &bounds,
                           const ioh::problem::ConstraintSet<T> &constraints,
                           const ioh::problem::Solution<T, ioh::problem::SingleObjective> &opt) :
            P(meta_data, bounds, constraints, opt)
        {
        }

        explicit PyProblem(const std::string &name, const int n_variables = 5, const int instance = 1,
                           const bool is_minimization = true,
                           ioh::problem::Bounds<T> bounds = ioh::problem::Bounds<T>(),
                           const ioh::problem::Constraints<T> &constraints = {},
                           const ioh::problem::Solution<T, ioh::problem::SingleObjective> &optimum = {}) :
            P(ioh::problem::MetaData(instance, name, n_variables,
                                     is_minimization ? ioh::common::OptimizationType::MIN
                                                     : ioh::common::OptimizationType::MAX),
              bounds, ioh::problem::ConstraintSet<T>(constraints),
              validate_optimum(optimum, n_variables, is_minimization))
        {
            [[maybe_unused]] auto registered = perform_registration();
        }

        // the overrides get x as a new numpy array, as the functions given to wrap_problem do
        double evaluate(const std::vector<T> &x) override
        {
            nb::gil_scoped_acquire gil;
            if (auto f = override_of("evaluate"); f.is_valid())
                return cast_result<double>(f(make_array(x)), "evaluate");
            throw std::runtime_error("Tried to call pure virtual function \"evaluate\"");
        }

        [[nodiscard]] std::vector<T> transform_variables(std::vector<T> x) override
        {
            {
                nb::gil_scoped_acquire gil;
                if (auto f = override_of("transform_variables"); f.is_valid())
                    return cast_result<InputVector<T>>(f(make_array(x)), "transform_variables");
            }
            return P::transform_variables(std::move(x));
        }

        [[nodiscard]] double transform_objectives(const double y) override
        {
            {
                nb::gil_scoped_acquire gil;
                if (auto f = override_of("transform_objectives"); f.is_valid())
                    return cast_result<double>(f(y), "transform_objectives");
            }
            return P::transform_objectives(y);
        }
    };

    //! __call__ for an input array (one solution): of floats for real problems, of any integer or bool type for
    //! integer problems
    template <typename ProblemType, typename T, typename Class>
    void define_array_calls(Class &c)
    {
        c.def("__call__", [](ProblemType &p, const InputArray1D<T> &x) { return p(to_vector<T>(x)); }, array_arg<T>());
    }

    //! __call__ for a two-dimensional input array, one solution per row; returns a new array of their values
    template <typename ProblemType, typename T, typename Class>
    void define_batch_array_calls(Class &c)
    {
        c.def(
            "__call__", [](ProblemType &p, const InputArray2D<T> &x) { return make_array(p(to_vectors<T>(x))); },
            array_arg<T>());
    }

    template <typename ProblemType, typename T>
    void define_base_class(nb::module_ &m, const std::string &name)
    {
        using Factory = ioh::common::Factory<ProblemType, int, int>;
        using ioh::problem::Bounds;
        using ioh::problem::Constraints;
        using ioh::problem::SingleObjective;
        using ioh::problem::Solution;

        nb::class_<ProblemType, PyProblem<ProblemType, T>> c(m, name.c_str());
        c.def(nb::init<const std::string, int, int, bool, Bounds<T>, Constraints<T>, Solution<T, SingleObjective>>(),
              nb::arg("name"), nb::arg("n_variables") = 5, nb::arg("instance") = 1, nb::arg("is_minimization") = true,
              nb::arg("bounds") = Bounds<T>(5), nb::arg("constraints") = Constraints<T>{},
              nb::arg("optimum") = Solution<T, SingleObjective>{},
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
            )pbdoc");

        // the arrays first, so that a numpy array is copied at once; lists take the std::vector overloads
        define_array_calls<ProblemType, T>(c);
        c.def("__call__", nb::overload_cast<const std::vector<T> &>(&ProblemType::operator()),
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
            )pbdoc");
        define_batch_array_calls<ProblemType, T>(c);
        c.def(
             "__call__", [](ProblemType &p, const std::vector<std::vector<T>> &x) { return make_array(p(x)); },
             R"pbdoc(
                Evaluate the problem at several search points.

                Parameters
                ----------
                    x: list[list]
                        the search points to evaluate, one per row. It must be a 2-dimensional array/list whose rows'
                        length matches search space's dimensionality
                Returns
                -------
                numpy.ndarray
                    The objective values of the search points: a new float64 array with one value per row
            )pbdoc")
            .def_static(
                "create",
                [](const std::string &name, int iid, int dim) { return Factory::instance().create(name, iid, dim); },
                nb::arg("problem_name"), nb::arg("instance_id"), nb::arg("dimension"),
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
                nb::arg("problem_id"), nb::arg("instance_id"), nb::arg("dimension"),
                R"pbdoc(
                Create a problem instance

                Parameters
                ----------
                    problem_id: the index of the problem to create.
                    instance_id: an integer identifier of the problem instance, which seeds the random generation
                        of the tranformations in the search/objective spaces
                    dimension: integer, representing the dimensionality of the search space
            )pbdoc")
            .def_prop_ro_static(
                "problems", [](nb::handle) { return Factory::instance().map(); }, "All registered problems")
            .def_prop_rw("log_info", &ProblemType::log_info, &ProblemType::set_log_info,
                         "The data is that being sent to the logger.")
            .def_prop_ro("state", &ProblemType::state,
                         "The current state of the optimization process containing, e.g., the current solution and "
                         "the number of function evaluated consumed so far")
            .def_prop_ro("meta_data", &ProblemType::meta_data,
                         "The static meta-data of the problem containing, e.g., problem id, instance id, and "
                         "problem's dimensionality")
            .def_prop_ro("optimum", &ProblemType::optimum,
                         "The optimum and its objective value for a problem instance")
            .def_prop_ro("bounds", &ProblemType::bounds, "The bounds of the problem.")
            .def_prop_ro("constraints", &ProblemType::constraints, "The constraints of the problem.")
            .def("add_constraint", &ProblemType::add_constraint, "add a constraint")
            .def("remove_constraint", &ProblemType::remove_constraint, "remove a constraint")
            .def("enforce_bounds", &ProblemType::enforce_bounds, nb::arg("weight") = 1.,
                 nb::arg("how") = ioh::problem::constraint::Enforced::SOFT, nb::arg("exponent") = 1.,
                 R"pbdoc(
                Enforced the bounds (box-constraints) as constraint
                Parameters
                ----------
                    weight: The weight for computing the penalty (can be infinity to have strict box-constraints)
                    how: The enforcement strategy, should be one of the 'ioh.ConstraintEnforcement' options
                    exponent: The exponent for scaling the contraint
                )pbdoc")

            .def("set_id", &ProblemType::set_id, nb::arg("new_problem_id"), "update the problem id")
            .def("set_instance", &ProblemType::set_instance, nb::arg("new_instance"), "update the problem instance")
            .def("set_name", &ProblemType::set_name, nb::arg("new_name"), "update the problem name")
            .def("set_final_target", &ProblemType::set_final_target, nb::arg("new_target"),
                 "update the final target")
            .def("invert", &ProblemType::invert)
            .def("__repr__", [=](const ProblemType &p) {
                const auto meta_data = p.meta_data();
                return "<" + name + fmt::format("Problem {:d}. ", meta_data.problem_id) + meta_data.name +
                    fmt::format(" (iid={:d} dim={:d})>", meta_data.instance, meta_data.n_variables);
            });
    }

    //! The value of a Python sequence (e.g. a numpy array) as a list
    inline nb::list as_list(nb::handle o)
    {
        PyObject *l = PySequence_List(o.ptr());
        if (!l)
            throw nb::python_error();
        return nb::steal<nb::list>(l);
    }

    //! A search point that a Python function returned (transform_variables, calculate_objective), as a vector of T:
    //! an array is read at once (InputVector), any other iterable as a list
    template <typename T>
    std::vector<T> cast_point(nb::handle value, const char *function)
    {
        if (InputVector<T> x; nb::try_cast(value, x))
            return std::vector<T>(std::move(x));
        return cast_result<std::vector<T>>(as_list(value), function);
    }

    template <typename T>
    void define_wrapper_functions(nb::module_ &m, const std::string &class_name, const std::string &function_name)
    {
        using WrappedProblem = ioh::problem::SingleObjectiveWrappedProblem<T>;
        using ioh::problem::Constraints;
        using ioh::problem::Solution;

        nb::class_<WrappedProblem, ioh::problem::SingleObjectiveProblem<T>>(m, class_name.c_str());

        m.def(
            function_name.c_str(),
            [](nb::handle f, const std::string &name, ioh::common::OptimizationType t, std::optional<double> lb,
               std::optional<double> ub, std::optional<nb::handle> tx, std::optional<nb::handle> ty,
               std::optional<nb::handle> co, Constraints<T> cs) {
                // the registry keeps the functions for the rest of the process
                register_python_fn(f);
                for (const auto &fn : {tx, ty, co})
                    if (fn)
                        register_python_fn(fn.value());

                auto of = [f](const std::vector<T> &x) {
                    nb::gil_scoped_acquire gil;
                    return cast_result<double>(f(make_array(x)), "the objective function");
                };

                auto ptx = [tx](std::vector<T> x, const int iid) {
                    if (tx)
                    {
                        nb::gil_scoped_acquire gil;
                        // a copy of x, which the function may keep; the transformed point is what it returns
                        std::vector<T> px = cast_point<T>(tx.value()(make_array(x), iid), "transform_variables");
                        if (px.size() == x.size())
                            return px;
                        nb::module_::import_("warnings")
                            .attr("warn")(fmt::format("Objective transformation function returned an iterable of "
                                                      "invalid length ({} != {}). Transformation is disabled.",
                                                      px.size(), x.size()));
                    }
                    return x;
                };

                auto pty = [ty](double y, const int iid) {
                    if (ty)
                    {
                        nb::gil_scoped_acquire gil;
                        nb::object py = ty.value()(y, iid);
                        const double result = PyFloat_AsDouble(py.ptr());
                        if (result == -1.0 && PyErr_Occurred())
                            throw nb::python_error();
                        return result;
                    }
                    return y;
                };

                auto pco = [co, t](const int iid, const int dim) {
                    if (co)
                    {
                        nb::gil_scoped_acquire gil;
                        nb::object xt = co.value()(iid, dim);
                        if (nb::isinstance<nb::iterable>(xt))
                        {
                            PyObject *tuple = PySequence_Tuple(xt.ptr());
                            if (!tuple)
                                throw nb::python_error();
                            auto args = nb::steal<nb::tuple>(tuple);
                            if (args.size() == 2)
                            {
                                PyObject *y = PyNumber_Float(args[1].ptr());
                                if (!y)
                                    throw nb::python_error();
                                return Solution<T, double>(cast_point<T>(args[0], "calculate_objective"),
                                                           nb::cast<double>(nb::steal(y)));
                            }
                        }
                        return cast_result<Solution<T, double>>(xt, "calculate_objective");
                    }
                    return Solution<T, double>(dim, t);
                };

                ioh::problem::wrap_function<T, double>(of, name, t, lb, ub, ptx, pty, pco, cs);
            },
            nb::arg("f"), nb::arg("name"), nb::arg("optimization_type") = ioh::common::OptimizationType::MIN,
            nb::arg("lb") = std::nullopt, nb::arg("ub") = std::nullopt, nb::arg("transform_variables") = std::nullopt,
            nb::arg("transform_objectives") = std::nullopt, nb::arg("calculate_objective") = std::nullopt,
            nb::arg("constraints") = Constraints<T>());
    }

    void define_problem_bases(nb::module_ &m)
    {
        define_base_class<ioh::problem::RealSingleObjective, double>(m, "RealSingleObjective");
        define_base_class<ioh::problem::IntegerSingleObjective, int>(m, "IntegerSingleObjective");
        define_wrapper_functions<double>(m, "RealSingleObjectiveWrappedProblem", "wrap_real_problem");
        define_wrapper_functions<int>(m, "IntegerSingleObjectiveWrappedProblem", "wrap_integer_problem");
    }
} // namespace ioh_python::problem

void define_problem(nb::module_ &m)
{
    ioh_python::problem::define_problem_bases(m);
    define_bbob(m);
    define_pbo_problems(m);
    define_cec2013_problems(m);
    define_cec2022_problems(m);
    define_dynamic_bin_val_problem(m);
    define_wmodels(m);
    define_submodular_problems(m);
    define_star_discrepancy_problems(m);
    define_many_affine(m);
    define_funnel(m);

    nb::module_::import_("atexit").attr("register")(nb::cpp_function([]() {
        // take the functions out under the lock, and release them outside it (a release can run Python code)
        std::vector<nb::handle> functions;
        {
            const std::lock_guard<std::mutex> lock(ioh_python::problem::wrapped_functions_mutex);
            functions.swap(ioh_python::problem::wrapped_functions);
        }
        for (const auto fn : functions)
            if (fn)
                fn.dec_ref();
    }));
}
