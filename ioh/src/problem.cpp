#include "pch.hpp"

using namespace ioh::problem;

static std::vector<nb::handle> WRAPPED_FUNCTIONS;

bool register_python_fn(nb::handle f)
{
    f.inc_ref();
    WRAPPED_FUNCTIONS.push_back(f);
    return true;
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
    using NBBase = P;
    nanobind::detail::trampoline<3> nb_trampoline{this};

    explicit PyProblem(const MetaData &meta_data, const Bounds<T> &bounds, const ConstraintSet<T> &constraints,
                       const Solution<T, SingleObjective> &opt) : P(meta_data, bounds, constraints, opt)
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

    double evaluate(const std::vector<T> &x) override { NB_OVERRIDE_PURE(evaluate, x); }

    [[nodiscard]] std::vector<T> transform_variables(std::vector<T> x) override { NB_OVERRIDE(transform_variables, x); }

    [[nodiscard]] double transform_objectives(const double y) override { NB_OVERRIDE(transform_objectives, y); }
};

template <typename ProblemType, typename T>
void define_base_class(nb::module_ &m, const std::string &name)
{
    using PyProblem = PyProblem<ProblemType, T>;
    using Factory = ioh::common::Factory<ProblemType, int, int>;

    // nb::options options;
    // options.disable_function_signatures();

    nb::class_<ProblemType, PyProblem>(m, name.c_str())
        .def(nb::init<const std::string, int, int, bool, Bounds<T>, Constraints<T>, Solution<T, SingleObjective>>(),
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
            )pbdoc")
        .def("__call__", nb::overload_cast<const std::vector<T> &>(&ProblemType::operator()),
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
        .def("__call__", nb::overload_cast<const std::vector<std::vector<T>> &>(&ProblemType::operator()),
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
            "problems", [](nb::object) { return Factory::instance().map(); }, "All registered problems")
        .def_prop_rw("log_info", &ProblemType::log_info, &ProblemType::set_log_info,
                     "The data is that being sent to the logger.")
        .def_prop_ro("state", &ProblemType::state,
                     "The current state of the optimization process containing, e.g., the current solution and the "
                     "number of function evaluated consumed so far")
        .def_prop_ro("meta_data", &ProblemType::meta_data,
                     "The static meta-data of the problem containing, e.g., problem id, instance id, and "
                     "problem's dimensionality")
        .def_prop_ro("optimum", &ProblemType::optimum, "The optimum and its objective value for a problem instance")
        .def_prop_ro("bounds", &ProblemType::bounds, "The bounds of the problem.")
        .def_prop_ro("constraints", &ProblemType::constraints, "The constraints of the problem.")
        .def("add_constraint", &ProblemType::add_constraint, "add a constraint")
        .def("remove_constraint", &ProblemType::remove_constraint, "remove a constraint")
        .def("enforce_bounds", &ProblemType::enforce_bounds, nb::arg("weight") = 1.,
             nb::arg("how") = constraint::Enforced::SOFT, nb::arg("exponent") = 1.,
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
        .def("set_final_target", &ProblemType::set_final_target, nb::arg("new_target"), "update the final target")
        .def("invert", &ProblemType::invert)
        .def("__repr__", [=](const ProblemType &p) {
            using namespace ioh::common;
            const auto meta_data = p.meta_data();
            return "<" + name + fmt::format("Problem {:d}. ", meta_data.problem_id) + meta_data.name +
                fmt::format(" (iid={:d} dim={:d})>", meta_data.instance, meta_data.n_variables);
        });
}


template <typename T>
void define_wrapper_functions(nb::module_ &m, const std::string &class_name, const std::string &function_name)
{
    using WrappedProblem = SingleObjectiveWrappedProblem<T>;
    nb::class_<WrappedProblem, SingleObjectiveProblem<T>>(m, class_name.c_str());

    m.def(
        function_name.c_str(),
        [](nb::handle f, const std::string &name, ioh::common::OptimizationType t, std::optional<double> lb,
           std::optional<double> ub, std::optional<nb::handle> tx, std::optional<nb::handle> ty,
           std::optional<nb::handle> co, Constraints<T> cs) {
            register_python_fn(f);

            auto of = [f](const std::vector<T> &x) {
                nb::gil_scoped_acquire gil;
                return nb::cast<double>(f(make_array(x)));
            };

            auto ptx = [tx](std::vector<T> x, const int iid) {
                if (tx)
                {
                    static bool r = register_python_fn(tx.value());

                    nb::gil_scoped_acquire gil;
                    nb::list px = nb::cast<nb::list>(tx.value()(make_mutable_array(x, nb::cast(&x)), iid));
                    if (px.size() == x.size())
                        return nb::cast<std::vector<T>>(px);
                    else
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
                    static bool r = register_python_fn(ty.value());
                    return PyFloat_AsDouble(ty.value()(y, iid).ptr());
                }
                return y;
            };

            auto pco = [co, t](const int iid, const int dim) {
                if (co)
                {
                    static bool r = register_python_fn(co.value());
                    nb::object xt = co.value()(iid, dim);

                    if (nb::isinstance<nb::iterable>(xt))
                    {
                        const auto args = nb::cast<nb::tuple>(xt);
                        if (args.size() == 2)
                        {
                            const auto x = nb::cast<std::vector<T>>(args[0]);
                            const auto y = nb::cast<double>(args[1]);
                            return Solution<T, double>(x, y);
                        }
                    }
                    return nb::cast<Solution<T, double>>(xt);
                }
                return Solution<T, double>(dim, t);
            };

            wrap_function<T, double>(of, name, t, lb, ub, ptx, pty, pco, cs);
        },
        nb::arg("f"), nb::arg("name"), 
        nb::arg("optimization_type") = ioh::common::OptimizationType::MIN,
        nb::arg("lb") = nb::none(), 
        nb::arg("ub") = nb::none(), 
        nb::arg("transform_variables") = nb::none(),
        nb::arg("transform_objectives") = nb::none(),
        nb::arg("calculate_objective") = nb::none(),
        nb::arg("constraints") = nb::list()
    );
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif


void define_problem_bases(nb::module_ &m)
{
    define_base_class<RealSingleObjective, double>(m, "RealSingleObjective");
    define_base_class<IntegerSingleObjective, int>(m, "IntegerSingleObjective");
    define_wrapper_functions<double>(m, "RealSingleObjectiveWrappedProblem", "wrap_real_problem");
    define_wrapper_functions<int>(m, "IntegerSingleObjectiveWrappedProblem", "wrap_integer_problem");
}


void define_bbob(nb::module_ &m);
void define_pbo_problems(nb::module_ &m);
void define_cec2013_problems(nb::module_ &m);
void define_cec2022_problems(nb::module_ &m);
void define_wmodels(nb::module_ &m);
void define_submodular_problems(nb::module_ &m);
void define_star_discrepancy_problems(nb::module_ &m);
void define_many_affine(nb::module_ &m);
void define_funnel(nb::module_ &m);
void define_dynamic_bin_val_problem(nb::module_ &m);


void define_problem(nb::module_ &m)
{
    define_problem_bases(m);
    define_bbob(m);
    define_pbo_problems(m);
    define_cec2013_problems(m);
    define_cec2022_problems(m);
    define_wmodels(m);
    define_submodular_problems(m);
    define_star_discrepancy_problems(m);
    define_many_affine(m);
    define_dynamic_bin_val_problem(m);
    define_funnel(m);

    nb::module_::import_("atexit").attr("register")(nb::cpp_function([]() {
        for (const auto fn : WRAPPED_FUNCTIONS)
            if (fn)
                fn.dec_ref();
    }));
}
