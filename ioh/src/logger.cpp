#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "ioh.hpp"

namespace py = pybind11;
using namespace ioh::logger;


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

    py::class_<Default, Base, std::shared_ptr<Default>>(m, "Default")
        .def(
            py::init<fs::path, std::string, std::string, std::string, bool, bool, int, int, bool,
            std::vector<int>, ioh::common::OptimizationType, int, int>(),
            py::arg("output_directory") = "./",
            py::arg("folder_name") = "ioh_data",
            py::arg("algorithm_name") = "algorithm_name",
            py::arg("algorithm_info") = "algorithm_info",
            py::arg("store_positions") = false,
            py::arg("t_always") = false,
            py::arg("t_on_interval") = 0,
            py::arg("t_per_time_range") = 0,
            py::arg("t_on_improvement") = true,
            py::arg("t_at_time_points") = std::vector<int>{0},
            py::arg("optimization_type") = ioh::common::OptimizationType::Minimization,
            py::arg("trigger_at_time_points_exp_base") = 10,
            py::arg("trigger_at_range_exp_base") = 10
        );

    py::class_<ECDF, Base, std::shared_ptr<ECDF>>(m, "ECDF")
        .def(py::init<double, double, size_t, size_t, size_t, size_t>())
        .def("data", &ECDF::data)
        .def("at", &ECDF::at)
        .def("error_range", &ECDF::error_range)
        .def("eval_range", &ECDF::eval_range)
        ;
};
