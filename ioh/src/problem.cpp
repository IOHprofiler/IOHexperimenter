#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "ioh.hpp"

namespace py = pybind11;
using namespace ioh::problem;

template <typename T>
void define_solution(py::module &m, const std::string &name)
{
    using Class = Solution<T>;

    py::options options;
    options.disable_function_signatures();

    py::class_<Class>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<std::vector<T>, std::vector<double>>(), R"pbdoc(
            Initialize a Solution object using its coordinates and fitness.

            Parameters
            ----------
                x: the coordinates in the searchspace
                y: the coordinates in the objective space (fitness value)
        )pbdoc")
        .def_readonly("x", &Class::x, "The coordinates in the searchspace.")
        .def_readonly("y", &Class::y, "The fitness value.");
}

template <typename T>
void define_state(py::module &m, const std::string &name)
{
    using Class = State<T>;
    using Class2 = Solution<T>;
    py::class_<Class>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<Class2>())
        .def_readonly("evaluations", &Class::evaluations, "The number of times the problem has been evaluated so far.")
        .def_readonly("optimum_found", &Class::optimum_found, "Boolean indicating whether or not the optimum has been found.")
        .def_readonly("current_best_internal", &Class::current_best_internal, "The internal representation of the best so far solution.")
        .def_readonly("current_best", &Class::current_best, "The current best-so-far solution.")
        .def_readonly("current_internal", &Class::current_internal, "The internal representation of the last-evaluated solution.")
        .def_readonly("current", &Class::current, "The last-evaluated solution.");
}

template <typename T>
void define_constraint(py::module &m, const std::string &name)
{
    using Class = Constraint<T>;

    py::options options;
    options.disable_function_signatures();

    py::class_<Class>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<std::vector<T>, std::vector<T>>())
        .def_readonly("ub", &Class::ub, "The upper bound (box constraint)")
        .def_readonly("lb", &Class::lb, "The lower bound (box constraint)")
        .def("check", &Class::check, R"pbdoc(
            Check if a point is inside the bounds or not.

            Parameters
            ----------
                x: The point for which to check the boundary conditions
        )pbdoc");
}

template <typename T>
void define_factory(py::module &m, const std::string &name)
{
    using Factory = ioh::common::Factory<T, int, int>;
    py::class_<Factory>(m, name.c_str(), py::buffer_protocol())
        .def("names", &Factory::names)
        .def("ids", &Factory::ids)
        .def("map", &Factory::map)
        .def("create", py::overload_cast<const int, int, int>(&Factory::create, py::const_),
             py::return_value_policy::reference)
        .def("create", py::overload_cast<const std::string&, int, int>(&Factory::create, py::const_),
             py::return_value_policy::reference);
}

template <typename P, typename T>
class PyProblem : public P
{
    [[nodiscard]]
    bool perform_registration()
    {
        auto meta_data = this->meta_data();
        auto constraint = this->constraint();
        ioh::common::Factory<P, int, int>::instance().include(
            meta_data.name, meta_data.problem_id,
            [=](const int instance, const int n_variables)
            {
                return std::make_shared<PyProblem<P, T>>(
                    MetaData(meta_data.problem_id, instance, meta_data.name,
                             n_variables, meta_data.n_objectives, meta_data.optimization_type),
                    constraint
                    );
            });
        return true;
    }

public:
    explicit PyProblem(const MetaData& meta_data, const Constraint<T>& constraint) :
        P(meta_data, constraint)
    {
    }

    explicit PyProblem(const std::string &name, const int n_variables = 5,
                       const int n_objectives = 1,
                       const int instance = 1, const bool is_minimization = true,
                       Constraint<T> constraint = Constraint<T>()) :
        P(MetaData(instance, name, n_variables, n_objectives, is_minimization
                   ? ioh::common::OptimizationType::Minimization
                   : ioh::common::OptimizationType::Maximization), constraint)
    {
        #if defined(__GNUC__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wunused-variable"
        #endif
        static auto registered = perform_registration();
        #if defined(__GNUC__)
        #pragma GCC diagnostic pop
        #endif
    }

    std::vector<double> evaluate(const std::vector<T> &x) override
    {
        PYBIND11_OVERRIDE_PURE(std::vector<double>, P, evaluate, x);
    }

    [[nodiscard]]
    std::vector<T> transform_variables(std::vector<T> x) override
    {
        PYBIND11_OVERRIDE(std::vector<T>, P, transform_variables, x);
    }

    [[nodiscard]]
    std::vector<double> transform_objectives(std::vector<double> y) override
    {
        PYBIND11_OVERRIDE(std::vector<double>, P, transform_objectives, y);
    }
};

template <typename ProblemType, typename T>
void define_base_class(py::module &m, const std::string &name)
{
    using PyProblem = PyProblem<ProblemType, T>;
    using Factory = ioh::common::Factory<ProblemType, int, int>;
    define_factory<ProblemType>(m, name + "Factory");

    py::options options;
    options.disable_function_signatures();

    py::class_<ProblemType, PyProblem, std::shared_ptr<ProblemType>>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<const std::string, int, int, int, bool, Constraint<T>>(),
             py::arg("name"),
             py::arg("n_variables") = 5,
             py::arg("n_objectives") = 1,
             py::arg("instance") = 1,
             py::arg("is_minimization") = true,
             py::arg("constraint") = Constraint<T>(5))
        .def("reset", &ProblemType::reset, R"pbdoc(
            Reset all state-variables of the problem.
        )pbdoc")
        .def("attach_logger", &ProblemType::attach_logger, R"pbdoc(
            Attach a logger to the problem to allow performance tracking.

            Parameters
            ----------
                logger: A logger-object from the IOHexperimenter 'logger' module.
        )pbdoc")
        .def("detach_logger", &ProblemType::detach_logger, R"pbdoc(
            Remove the specified logger from the problem.

            Parameters
            ----------
                logger: A logger-object from the IOHexperimenter 'logger' module.
        )pbdoc")
        .def("__call__", &ProblemType::operator(), R"pbdoc(
            Evaluate the problem.

            Parameters
            ----------
                x: a 1-dimensional array / list of size equal to the dimension of this problem
        )pbdoc")
        .def_static("factory", &Factory::instance, py::return_value_policy::reference, "A factory method to get the relevant problem. Recommended is to use the 'get_problem'-function instead.")
        .def_property_readonly("log_info", &ProblemType::log_info, "Check what data is being sent to the logger.")
        .def_property_readonly("state", &ProblemType::state, "The current state of the problem: all variables which change during the optimization procedure.")
        .def_property_readonly("meta_data", &ProblemType::meta_data, "The meta-data of the problem: these variables are static during the lifetime of the problem.")
        .def_property_readonly("objective", &ProblemType::objective, "The optimal point and value for the current instanciation of the problem.")
        .def_property_readonly("constraint", &ProblemType::constraint, "The constraints (bounds) of the problem.")
        .def("__repr__", [=](const ProblemType &p)
        {
            using namespace ioh::common;
            const auto meta_data = p.meta_data();
            return "<" + name + string_format("Problem %d. ", meta_data.problem_id) +
                meta_data.name + string_format(" (%d %d)>", meta_data.instance, meta_data.n_variables);
        });
}

template <typename T>
void define_wrapper_functions(py::module &m, const std::string &class_name, const std::string &function_name)
{
    using WrappedProblem = WrappedProblem<T>;
    py::class_<WrappedProblem, Problem<T>, std::shared_ptr<WrappedProblem>>(m, class_name.c_str(), py::buffer_protocol());
    m.def(function_name.c_str(), &wrap_function<T>,
          py::arg("f"),
          py::arg("name"),
          py::arg("n_variables") = 5,
          py::arg("n_objectives") = 1,
          py::arg("optimization_type") = ioh::common::OptimizationType::Minimization,
          py::arg("constraint") = Constraint<T>(5),
          py::return_value_policy::reference
        );
}

void define_helper_classes(py::module &m)
{
    py::enum_<ioh::common::OptimizationType>(m, "OptimizationType")
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
        .def(py::init<int, int, std::string, int, int, ioh::common::OptimizationType>())
        .def_readonly("instance", &MetaData::instance, "The instance number of the current problem")
        .def_readonly("problem_id", &MetaData::problem_id, "The id of the problem within its suite")
        .def_readonly("name", &MetaData::name, "The name of the current problem")
        .def_readonly("optimization_type", &MetaData::optimization_type, "The type of problem (maximization or minimization)")
        .def_readonly("n_variables", &MetaData::n_variables, "The number of variables (dimension) of the current problem")
        .def_readonly("n_objectives", &MetaData::n_objectives, "The number of objectives of the current problem")
        .def_readonly("initial_objective_value", &MetaData::initial_objective_value); //  What does this variable mean?

    py::class_<ioh::logger::LogInfo>(m, "LogInfo")
        .def(py::init<size_t, double, double, double, Solution<double>, Solution<double>>())
        .def_readonly("evaluations", &ioh::logger::LogInfo::evaluations, "The number of evaluations performed on the current problem so far")
        .def_readonly("y_best", &ioh::logger::LogInfo::y_best, "The best fitness value found so far")
        .def_readonly("transformed_y", &ioh::logger::LogInfo::transformed_y, "The internal representation of the current fitness value")
        .def_readonly("transformed_y_best", &ioh::logger::LogInfo::transformed_y_best, "The internal representation of the best-so-far fitness")
        .def_readonly("current", &ioh::logger::LogInfo::current, "The fitness of the last evaluated solution")
        .def_readonly("objective", &ioh::logger::LogInfo::objective, "The best possible fitness value");
}

void define_pbo_problems(py::module& m)
{
    define_factory<PBO>(m, "PBOFactory");
    py::class_<PBO, Integer, std::shared_ptr<PBO>>(
        m, "PBO",
        R"pbdoc(
            Pseudo-Boolean Optimization (PBO) problem set, which contains 25 test functions taking
            their domain on {0, 1}^n, where n is the length of bitstrings.

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

        )pbdoc"
    )
        .def_static("factory", &ioh::common::Factory<PBO, int, int>::instance, py::return_value_policy::reference)
    ;
    py::class_<pbo::OneMax, Integer, std::shared_ptr<pbo::OneMax>>(
        m, "OneMax", py::is_final(),
        R"pbdoc(
            OneMax:
            {0,1}^n → [0..n], x ↦ ∑_{i=1}^n x_i.

        )pbdoc")
        .def(py::init<int, int>());
    py::class_<pbo::LeadingOnes, Integer, std::shared_ptr<pbo::LeadingOnes>>(
        m, "LeadingOnes", py::is_final(),
        R"pbdoc(
            LeadingOnes:
            {0,1}^n → [0..n], x ↦ max{i∈[0..n] ∣ ∀j≤i: x_j=1}

        )pbdoc")
        .def(py::init<int, int>());
    py::class_<pbo::Linear, Integer, std::shared_ptr<pbo::Linear>>(
        m, "Linear", py::is_final(),
        R"pbdoc(
            A Linear Function with Harmonic Weights:
            {0,1}^n → ℝ, x ↦ ∑_i i * x_i

        )pbdoc")
        .def(py::init<int, int>());
    py::class_<pbo::OneMaxDummy1, Integer, std::shared_ptr<pbo::OneMaxDummy1>>(m, "OneMaxDummy1", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::OneMaxDummy2, Integer, std::shared_ptr<pbo::OneMaxDummy2>>(m, "OneMaxDummy2", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::OneMaxNeutrality, Integer, std::shared_ptr<pbo::OneMaxNeutrality>>(m, "OneMaxNeutrality", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::OneMaxEpistasis, Integer, std::shared_ptr<pbo::OneMaxEpistasis>>(m, "OneMaxEpistasis", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::OneMaxRuggedness1, Integer, std::shared_ptr<pbo::OneMaxRuggedness1>>(m, "OneMaxRuggedness1", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::OneMaxRuggedness2, Integer, std::shared_ptr<pbo::OneMaxRuggedness2>>(m, "OneMaxRuggedness2", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::OneMaxRuggedness3, Integer, std::shared_ptr<pbo::OneMaxRuggedness3>>(m, "OneMaxRuggedness3", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::LeadingOnesDummy1, Integer, std::shared_ptr<pbo::LeadingOnesDummy1>>(m, "LeadingOnesDummy1", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::LeadingOnesDummy2, Integer, std::shared_ptr<pbo::LeadingOnesDummy2>>(m, "LeadingOnesDummy2", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::LeadingOnesNeutrality, Integer, std::shared_ptr<pbo::LeadingOnesNeutrality>>(m, "LeadingOnesNeutrality", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::LeadingOnesEpistasis, Integer, std::shared_ptr<pbo::LeadingOnesEpistasis>>(m, "LeadingOnesEpistasis", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::LeadingOnesRuggedness1, Integer, std::shared_ptr<pbo::LeadingOnesRuggedness1>>(m, "LeadingOnesRuggedness1", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::LeadingOnesRuggedness2, Integer, std::shared_ptr<pbo::LeadingOnesRuggedness2>>(m, "LeadingOnesRuggedness2", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::LeadingOnesRuggedness3, Integer, std::shared_ptr<pbo::LeadingOnesRuggedness3>>(m, "LeadingOnesRuggedness3", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::LABS, Integer, std::shared_ptr<pbo::LABS>>(
        m, "LABS", py::is_final(),
        R"pbdoc(
            Low Autocorrelation Binary Sequences (LABS):
            x ↦ n^2 / 2∑_{k=1}^{n-1}(∑_{i=1}^{n−k}s_is_{i+k})^2, where s_i = 2x_i − 1

        )pbdoc"
    )
        .def(py::init<int, int>());
    py::class_<pbo::IsingRing, Integer, std::shared_ptr<pbo::IsingRing>>(m, "IsingRing", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::IsingTorus, Integer, std::shared_ptr<pbo::IsingTorus>>(m, "IsingTorus", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::IsingTriangular, Integer, std::shared_ptr<pbo::IsingTriangular>>(m, "IsingTriangular", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::MIS, Integer, std::shared_ptr<pbo::MIS>>(m, "MIS", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::NQueens, Integer, std::shared_ptr<pbo::NQueens>>(m, "NQueens", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::ConcatenatedTrap, Integer, std::shared_ptr<pbo::ConcatenatedTrap>>(m, "ConcatenatedTra", py::is_final())
        .def(py::init<int, int>());
}

void define_bbob_problems(py::module& m)
{
    define_factory<BBOB>(m, "BBOBFactory");
    py::class_<BBOB, Real, std::shared_ptr<BBOB>>(
        m, "BBOB",
        R"pbdoc(
            Black-Box Optimization Benchmarking (BBOB) problem set, which contains 24 noiseless
            real-valued test functions supported on [-5, 5]^n, where n is the dimensionality.

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

        )pbdoc"
    )
        .def_static("factory", &ioh::common::Factory<BBOB, int, int>::instance, py::return_value_policy::reference)
    ;
    py::class_<bbob::Sphere, Real, std::shared_ptr<bbob::Sphere>>(m, "Sphere", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::Ellipsoid, Real, std::shared_ptr<bbob::Ellipsoid>>(m, "Ellipsoid", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::Rastrigin, Real, std::shared_ptr<bbob::Rastrigin>>(m, "Rastrigin", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::BuecheRastrigin, Real, std::shared_ptr<bbob::BuecheRastrigin>>(m, "BuecheRastrigin", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::LinearSlope, Real, std::shared_ptr<bbob::LinearSlope>>(m, "LinearSlope", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::AttractiveSector, Real, std::shared_ptr<bbob::AttractiveSector>>(m, "AttractiveSector", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::StepEllipsoid, Real, std::shared_ptr<bbob::StepEllipsoid>>(m, "StepEllipsoid", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::Rosenbrock, Real, std::shared_ptr<bbob::Rosenbrock>>(m, "Rosenbrock", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::RosenbrockRotated, Real, std::shared_ptr<bbob::RosenbrockRotated>>(m, "RosenbrockRotated", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::EllipsoidRotated, Real, std::shared_ptr<bbob::EllipsoidRotated>>(m, "EllipsoidRotated", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::Discus, Real, std::shared_ptr<bbob::Discus>>(m, "Discus", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::BentCigar, Real, std::shared_ptr<bbob::BentCigar>>(m, "BentCigar", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::SharpRidge, Real, std::shared_ptr<bbob::SharpRidge>>(m, "SharpRidge", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::DifferentPowers, Real, std::shared_ptr<bbob::DifferentPowers>>(m, "DifferentPowers", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::RastriginRotated, Real, std::shared_ptr<bbob::RastriginRotated>>(m, "RastriginRotated", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::Weierstrass, Real, std::shared_ptr<bbob::Weierstrass>>(m, "Weierstrass", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::Schaffers10, Real, std::shared_ptr<bbob::Schaffers10>>(m, "Schaffers10", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::Schaffers1000, Real, std::shared_ptr<bbob::Schaffers1000>>(m, "Schaffers1000", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::GriewankRosenBrock, Real, std::shared_ptr<bbob::GriewankRosenBrock>>(m, "GriewankRosenBrock", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::Schwefel, Real, std::shared_ptr<bbob::Schwefel>>(m, "Schwefel", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::Gallagher101, Real, std::shared_ptr<bbob::Gallagher101>>(m, "Gallagher101", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::Gallagher21, Real, std::shared_ptr<bbob::Gallagher21>>(m, "Gallagher21", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::Katsuura, Real, std::shared_ptr<bbob::Katsuura>>(m, "Katsuura", py::is_final())
        .def(py::init<int, int>());
    py::class_<bbob::LunacekBiRastrigin, Real, std::shared_ptr<bbob::LunacekBiRastrigin>>(m, "LunacekBiRastrigin", py::is_final())
        .def(py::init<int, int>());
}

void define_problem_bases(py::module &m)
{
    define_base_class<Real, double>(m, "Real");
    define_base_class<Integer, int>(m, "Integer");
    define_wrapper_functions<double>(m, "RealWrappedProblem", "wrap_real_problem");
    define_wrapper_functions<int>(m, "IntegerWrappedProblem", "wrap_integer_problem");
}

void define_problem(py::module &m)
{
    define_problem_bases(m);
    define_bbob_problems(m);
    define_pbo_problems(m);
}
