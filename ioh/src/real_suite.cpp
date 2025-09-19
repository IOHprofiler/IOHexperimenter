#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "numpy.hpp"
#include "ioh.hpp"

namespace py = pybind11;
using namespace ioh::suite;

std::string make_docstring(const std::string &name);

template <typename SuiteType>
static void define_base_class(py::module &m, const std::string &name)
{
    py::class_<SuiteType, std::shared_ptr<SuiteType>>(m, name.c_str(), py::buffer_protocol(),
                                                      fmt::format("{} suite", name).c_str())
        .def("reset", &SuiteType::reset,
             R"pbdoc(
                Reset the state variables of the current problem.
            )pbdoc")
        .def("attach_logger", &SuiteType::attach_logger,
             R"pbdoc(
                Attach a logger to the problem to track the performance and internal variables.
            )pbdoc")
        .def("detach_logger", &SuiteType::detach_logger,
             R"pbdoc(
                Remove the specified logger from the problem
            )pbdoc")
        .def_property_readonly(
            "problem_ids",
            [](const SuiteType &c) {
                const auto problem_ids = c.problem_ids();
                return make_array(problem_ids);
            },
            R"pbdoc(
                The list of all problems ids contained in the current suite.
            )pbdoc")
        .def_property_readonly(
            "dimensions",
            [](const SuiteType &c) {
                const auto dimensions = c.dimensions();
                return make_array(dimensions);
            },
            R"pbdoc(
                The list of all problems ids contained in the current suite.
            )pbdoc")
        .def_property_readonly(
            "instances",
            [](const SuiteType &c) {
                const auto instances = c.instances();
                return make_array(instances);
            },
            R"pbdoc(
                The list of all instance ids contained in the current suite.
            )pbdoc")
        .def_property_readonly("name", &SuiteType::name,
                               R"pbdoc(
                The name of the suite.
            )pbdoc")
        .def("__len__", &SuiteType::size)
        .def(
            "__iter__", [](SuiteType &s) { return py::make_iterator(s.begin(), s.end()); }, py::keep_alive<0, 1>());
}


template <typename S, typename P>
static void define_suite(py::module &m, const std::string &name, const int default_dim = 5)
{
    py::class_<S, P, std::shared_ptr<S>>(m, name.c_str(), make_docstring(name).c_str())
        .def(py::init<std::vector<int>, std::vector<int>, std::vector<int>>(), py::arg("problem_ids"),
             py::arg("instances") = std::vector<int>{1}, py::arg("dimensions") = std::vector<int>{default_dim});
}


void define_real_suite(py::module &m)
{
    using RealBase = Suite<ioh::problem::RealSingleObjective>;
    define_base_class<RealBase>(m, "RealBase");
    define_suite<Real, RealBase>(m, "Real");
    define_suite<BBOB, RealBase>(m, "BBOB");
    define_suite<SBOX, RealBase>(m, "SBOX");
    define_suite<RealStarDiscrepancy, RealBase>(m, "RealStarDiscrepancy");
}