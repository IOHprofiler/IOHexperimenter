#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "ioh.hpp"

namespace py = pybind11;
using namespace ioh::logger;


class PyLogger final: public Default
{
    py::object attribute_container_{};
    std::vector<std::string> logged_attribute_names_{};
    std::vector<std::string> run_attribute_names_{};


    std::vector<double> get_attributes(const std::vector<std::string>& names) const 
    {
        std::vector<double> attributes;
        for (const auto &n : names)
            attributes.emplace_back(
                PyFloat_AsDouble(attribute_container_.attr(n.c_str()).ptr())
            );
        return attributes;
    }

public:
    using Default::Default;

    void declare_logged_attributes(py::object &obj, const std::vector<std::string> &names)
    {
        attribute_container_ = obj;
        logged_attribute_names_ = names;
        create_logged_attributes(logged_attribute_names_);
    }

    void declare_run_attributes(py::object &obj, const std::vector<std::string> &names)
    {
        attribute_container_ = obj;
        run_attribute_names_ = names;
        create_run_attributes(run_attribute_names_);
    } 

    void declare_experiment_attributes(const std::vector<std::string> &names, const std::vector<double> &values)
    {
        for (size_t i = 0; i < names.size(); i++)
            add_experiment_attribute(names.at(i), values.at(i));
    }

    void track_problem(const ioh::problem::MetaData &problem) override
    {
        set_run_attributes(run_attribute_names_, get_attributes(run_attribute_names_));
        Default::track_problem(problem);
    }

    void log(const LogInfo& log_info) override
    {
        set_logged_attributes(logged_attribute_names_, get_attributes(logged_attribute_names_));
        Default::log(log_info);
    }
};
        
        

void define_logger(py::module &m)
{
    py::class_<fs::path>(m, "Path")
        .def(py::init<std::string>());
    py::implicitly_convertible<std::string, fs::path>();

    py::class_<Base, std::shared_ptr<Base>>(m, "Base")
        .def(
            "track_problem", &Base::track_problem,
            R"pbdoc(
                Track a specific problem with this logger.
            )pbdoc"
        )
        .def(
            "track_suite", &Base::track_suite,
            R"pbdoc(
                Track a specific suite with this logger
            )pbdoc"
        )
        .def(
            "log", &Base::log,
            R"pbdoc(
                Log the specified data.
            )pbdoc"
        )
        .def(
            "flush", &Base::flush,
            R"pbdoc(
                Flush the logger (finishes all open file-writing actions).
            )pbdoc"
        );

    py::class_<LoggerCombine, Base, std::shared_ptr<LoggerCombine>>(m, "LoggerCombine")
        .def(py::init<Base&>())
        .def(py::init<std::vector<Base*>>())
        .def("add", &LoggerCombine::add)
        ;

    py::class_<PyLogger, Base, std::shared_ptr<PyLogger>>(m, "Default")
        .def(py::init<fs::path, std::string, std::string, std::string, ioh::common::OptimizationType, bool, bool, int,
                      int, bool, std::vector<int>, int, int>(),
             py::arg("output_directory") = "./", py::arg("folder_name") = "ioh_data",
             py::arg("algorithm_name") = "algorithm_name", py::arg("algorithm_info") = "algorithm_info",
             py::arg("optimization_type") = ioh::common::OptimizationType::Minimization,
             py::arg("store_positions") = false, py::arg("t_always") = false, py::arg("t_on_interval") = 0,
             py::arg("t_per_time_range") = 0, py::arg("t_on_improvement") = true,
             py::arg("t_at_time_points") = std::vector<int>{0}, py::arg("trigger_at_time_points_exp_base") = 10,
             py::arg("trigger_at_range_exp_base") = 10)
        .def("declare_experiment_attributes", &PyLogger::declare_experiment_attributes)
        .def("declare_run_attributes", &PyLogger::declare_run_attributes)
        .def("declare_logged_attributes", &PyLogger::declare_logged_attributes)
        .def("__enter__", [=](const PyLogger &self) {return &self;})
        .def("__exit__", [=](PyLogger& self, const py::object &, const py::object &, const py::object &){
            self.flush();
        })
        .def("__repr__", [=](PyLogger &p){
            return "<DefaultLogger: '" + p.experiment_folder().path().generic_string() + "'>"; 
        })
        ;
    py::class_<ECDF, Base, std::shared_ptr<ECDF>>(m, "ECDF")
        .def(py::init<double, double, size_t, size_t, size_t,  size_t>())
        .def("data", &ECDF::data)
        .def("at", &ECDF::at)
        .def("error_range", &ECDF::error_range)
        .def("eval_range", &ECDF::eval_range)
        ;
}
