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
    py::class_<Class>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<std::vector<T>, std::vector<double>>())
        .def_readonly("x", &Class::x)
        .def_readonly("y", &Class::y);
}

template <typename T>
void define_state(py::module &m, const std::string &name)
{
    using Class = State<T>;
    using Class2 = Solution<T>;
    py::class_<Class>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<Class2>())
        .def_readonly("evaluations", &Class::evaluations)
        .def_readonly("optimum_found", &Class::optimum_found)
        .def_readonly("current_best_internal", &Class::current_best_internal)
        .def_readonly("current_best", &Class::current_best)
        .def_readonly("current_internal", &Class::current_internal)
        .def_readonly("current", &Class::current);
}

template <typename T>
void define_constraint(py::module &m, const std::string &name)
{
    using Class = Constraint<T>;
    py::class_<Class>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<std::vector<T>, std::vector<T>>())
        .def_readonly("ub", &Class::ub)
        .def_readonly("lb", &Class::lb)
        .def("check", &Class::check);
}

template <typename T>
void define_factory(py::module &m, const std::string &name)
{
    using Factory = ioh::common::Factory<T, int, int>;
    py::class_<Factory>(m, name.c_str(), py::buffer_protocol())
        .def("names", &Factory::names)
        .def("ids", &Factory::ids)
        .def("name_to_id", &Factory::name_to_id)
        .def("create", py::overload_cast<const int, int, int>(&Factory::create, py::const_),
             py::return_value_policy::reference)
        .def("create", py::overload_cast<const std::string, int, int>(&Factory::create, py::const_),
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
        static auto registered = perform_registration();
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

    py::class_<ProblemType, PyProblem, std::shared_ptr<ProblemType>>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<const std::string, int, int, int, bool, Constraint<T>>(),
             py::arg("name"),
             py::arg("n_variables") = 5,
             py::arg("n_objectives") = 1,
             py::arg("instance") = 1,
             py::arg("is_minimization") = true,
             py::arg("constraint") = Constraint<T>(5))
        .def("reset", &ProblemType::reset)
        .def("attach_logger", &ProblemType::attach_logger)
        .def("detach_logger", &ProblemType::detach_logger)
        .def("__call__", &ProblemType::operator())
        .def_static("factory", &Factory::instance, py::return_value_policy::reference)
        .def_property_readonly("log_info", &ProblemType::log_info)
        .def_property_readonly("state", &ProblemType::meta_data)
        .def_property_readonly("meta_data", &ProblemType::meta_data)
        .def_property_readonly("objective", &ProblemType::objective)
        .def_property_readonly("constraint", &ProblemType::state)
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
    py::class_<WrappedProblem, Problem<T>, std::shared_ptr<WrappedProblem>>(
        m, class_name.c_str(), py::buffer_protocol());
    m.def(function_name.c_str(), &wrap_function<double>,
          py::arg("f"),
          py::arg("name"),
          py::arg("n_variables") = 5,
          py::arg("n_objectives") = 1,
          py::arg("optimization_type") = ioh::common::OptimizationType::Minimization,
          py::arg("constraint") = Constraint<T>(5));
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
        .def_readonly("instance", &MetaData::instance)
        .def_readonly("problem_id", &MetaData::problem_id)
        .def_readonly("name", &MetaData::name)
        .def_readonly("optimization_type", &MetaData::optimization_type)
        .def_readonly("n_variables", &MetaData::n_variables)
        .def_readonly("n_objectives", &MetaData::n_objectives)
        .def_readonly("initial_objective_value", &MetaData::initial_objective_value);

    py::class_<ioh::logger::LogInfo>(m, "LogInfo")
        .def(py::init<size_t, double, double, double, Solution<double>, Solution<double>>())
        .def_readonly("evaluations", &ioh::logger::LogInfo::evaluations)
        .def_readonly("y_best", &ioh::logger::LogInfo::y_best)
        .def_readonly("transformed_y", &ioh::logger::LogInfo::transformed_y)
        .def_readonly("transformed_y_best", &ioh::logger::LogInfo::transformed_y_best)
        .def_readonly("current", &ioh::logger::LogInfo::current)
        .def_readonly("objective", &ioh::logger::LogInfo::objective);
}

void define_pbo_problems(py::module& m)
{
    py::class_<PBO, Integer, std::shared_ptr<PBO>>(m, "PBO");
    py::class_<pbo::OneMax, Integer, std::shared_ptr<pbo::OneMax>>(m, "OneMax", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::LeadingOnes, Integer, std::shared_ptr<pbo::LeadingOnes>>(m, "LeadingOnes", py::is_final())
        .def(py::init<int, int>());
    py::class_<pbo::Linear, Integer, std::shared_ptr<pbo::Linear>>(m, "Linear", py::is_final())
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
    py::class_<pbo::LABS, Integer, std::shared_ptr<pbo::LABS>>(m, "LABS", py::is_final())
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
    py::class_<BBOB, Real, std::shared_ptr<BBOB>>(m, "BBOB");
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
