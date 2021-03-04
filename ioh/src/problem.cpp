#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "ioh.hpp"

namespace py = pybind11;
using namespace ioh::problem;

template<typename T>
void define_solution(py::module& m, const std::string& name)
{
    using Class = Solution<T>;
    py::class_<Class>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<std::vector<T>, std::vector<double>>())
        .def_readonly("x", &Class::x)
        .def_readonly("y", &Class::y);
}

template<typename T>
void define_state(py::module& m, const std::string& name)
{
    using Class = State<T>;
    using Class2 = Solution<T>;
    py::class_<Class>(m, name.c_str(), py::buffer_protocol())
        .def(py::init <Class2> ())
        .def_readonly("evaluations", &Class::evaluations)
        .def_readonly("optimum_found", &Class::optimum_found)
        .def_readonly("current_best_internal", &Class::current_best_internal)
        .def_readonly("current_best", &Class::current_best)
        .def_readonly("current_internal", &Class::current_internal)
        .def_readonly("current", &Class::current);
}

template<typename T>
void define_constraint(py::module& m, const std::string& name)
{
    using Class = Constraint<T>;
    py::class_<Class>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<std::vector<T>, std::vector<T>>())
        .def_readonly("ub", &Class::ub)
        .def_readonly("lb", &Class::lb)
        .def("check", &Class::check);

}

template<typename T>
void define_factory(py::module &m, const std::string& name)
{
    using Factory = ioh::common::Factory<T, int, int>;
    py::class_<Factory>(m, name.c_str(), py::buffer_protocol())
        .def("instance", &Factory::instance, py::return_value_policy::reference)
        .def("names", &Factory::names)
        .def("ids", &Factory::ids)
        .def("name_to_id", &Factory::name_to_id)
        .def("create", py::overload_cast<const int, int, int>(&Factory::create, py::const_),
            py::return_value_policy::reference)
        .def("create", py::overload_cast<const std::string, int, int>(&Factory::create, py::const_),
            py::return_value_policy::reference)
        ;
}

template<typename Base>
void define_base_class(py::module& m, const std::string& name)
{
    py::class_<Base, std::shared_ptr<Base>>(m, name.c_str(), py::buffer_protocol())
        .def("reset", &Base::reset)
        .def("log_info", &Base::log_info)
        .def("attach_logger", &Base::attach_logger)
        .def("detach_logger", &Base::detach_logger)
        .def("__call__", &Base::operator())
        .def("state", &Base::meta_data)
        .def("meta_data", &Base::meta_data)
        .def("objective", &Base::objective)
        .def("constraint", &Base::state)
        .def("__repr__", [=](const Base& p)
            {
                using namespace ioh::common;
                const auto meta_data = p.meta_data();
                return "<" + name + string_format("Problem %d. ", meta_data.problem_id) +
                    meta_data.name + string_format(" (%d %d)>", meta_data.instance, meta_data.n_variables);
            });
}

template<typename T>
void define_wrapper_functions(py::module& m, const std::string& class_name, const std::string& function_name)
{
    using WrappedProblem = WrappedProblem<T>;
    py::class_<WrappedProblem, Problem<T>, std::shared_ptr<WrappedProblem>>(m, class_name.c_str(), py::buffer_protocol());
    m.def(function_name.c_str(), &wrap_function<double>,
        py::arg("f"),
        py::arg("name"),
        py::arg("n_variables") = 5,
        py::arg("n_objectives") = 1,
        py::arg("optimization_type") = ioh::common::OptimizationType::Minimization,
        py::arg("constraint") = Constraint<T>(5));
}

void define_helper_classes(py::module& m)
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

    py::class_<ioh::logger::Base>(m, "LoggerBase")
        .def("track_problem", &ioh::logger::Base::track_problem)
        .def("track_suite", &ioh::logger::Base::track_suite)
        .def("log", &ioh::logger::Base::log)
        .def("flush", &ioh::logger::Base::flush);
}


void define_problem_bases(py::module &m)
{
  
    define_base_class<Real>(m, "Real");
    define_base_class<Integer>(m, "Integer");
    define_wrapper_functions<double>(m, "RealWrappedProblem", "wrap_real_problem");
    define_wrapper_functions<int>(m, "IntegerWrappedProblem", "wrap_integer_problem");
    define_factory<Real>(m, "RealFactory");
    define_factory<Integer>(m, "IntegerFactory");

    py::class_<BBOB, Real, std::shared_ptr<BBOB>>(m, "BBOB");
    py::class_<PBO, Integer, std::shared_ptr<PBO>>(m, "PBO");
}

void init_problem(py::module &m)
{
    define_helper_classes(m);
    define_problem_bases(m);
}
