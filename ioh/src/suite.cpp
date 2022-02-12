#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "ioh.hpp"

namespace py = pybind11;
using namespace ioh::suite;

template <typename SuiteType>
void define_base_class(py::module &m, const std::string &name)
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
        .def_property_readonly("problem_ids", &SuiteType::problem_ids,
                               R"pbdoc(
                The list of all problems ids contained in the current suite.
            )pbdoc")
        .def_property_readonly("dimensions", &SuiteType::dimensions,
                               R"pbdoc(
                The list of all problems ids contained in the current suite.
            )pbdoc")
        .def_property_readonly("instances", &SuiteType::instances,
                               R"pbdoc(
                The list of all instance ids contained in the current suite.
            )pbdoc") // Should this be &SuiteType::instance_ids?
        .def_property_readonly("name", &SuiteType::name,
                               R"pbdoc(
                The name of the suite.
            )pbdoc")
        .def("__len__", &SuiteType::size)
        .def(
            "__iter__", [](SuiteType &s) { return py::make_iterator(s.begin(), s.end()); }, py::keep_alive<0, 1>());
}


void define_suite(py::module &m)
{
    define_base_class<Suite<ioh::problem::Real>>(m, "RealBase");

    py::class_<Real, Suite<ioh::problem::Real>, std::shared_ptr<Real>>(m, "Real",
                                                                       R"pbdoc(
        Suite with real-valued functions included in IOH. 

        Parameters
        ----------
        problem_ids: list[int]
            A list of problems ids to include in this instantiation of the suite
        instances: list[int]
            A list of problem instances to include in this instantiation of the suite
        problem_ids: list[int]
            A list of problem dimensions to include in this instantiation of the suite          
    )pbdoc")
        .def(py::init<std::vector<int>, std::vector<int>, std::vector<int>>(), py::arg("problem_ids"),
             py::arg("instances"), py::arg("dimensions"));

    py::class_<BBOB, Suite<ioh::problem::Real>, std::shared_ptr<BBOB>>(m, "BBOB", R"pbdoc(
            Suite with real-valued functions from BBOB single-objective benchmark.

            Parameters
            ----------
            problem_ids: list[int]
                A list of problems ids to include in this instantiation of the suite
            instances: list[int]
                A list of problem instances to include in this instantiation of the suite
            problem_ids: list[int]
                A list of problem dimensions to include in this instantiation of the suite   
                      
        )pbdoc")
        .def(py::init<std::vector<int>, std::vector<int>, std::vector<int>>(), py::arg("problem_ids"),
             py::arg("instances"), py::arg("dimensions"));

    define_base_class<Suite<ioh::problem::Integer>>(m, "IntegerBase");

    py::class_<Integer, Suite<ioh::problem::Integer>, std::shared_ptr<Integer>>(m, "Integer",
                                                                                R"pbdoc(
            Suite with integer-valued functions included in IOH. 

            Parameters
            ----------
            problem_ids: list[int]
                A list of problems ids to include in this instantiation of the suite
            instances: list[int]
                A list of problem instances to include in this instantiation of the suite
            problem_ids: list[int]
                A list of problem dimensions to include in this instantiation of the suite          
        )pbdoc")
        .def(py::init<std::vector<int>, std::vector<int>, std::vector<int>>(), py::arg("problem_ids"),
             py::arg("instances"), py::arg("dimensions"));

    py::class_<PBO, Suite<ioh::problem::Integer>, std::shared_ptr<PBO>>(m, "PBO",
                                                                        R"pbdoc(
            Suite with integer-valued functions from PBO single objective benchmark.

            This class is an iterator for problems, you can define a set of problem/instance/dimension
            combinations to iterate over.

            Parameters
            ----------
            problem_ids: list[int]
                A list of problems ids to include in this instantiation of the suite
            instances: list[int]
                A list of problem instances to include in this instantiation of the suite
            problem_ids: list[int]
                A list of problem dimensions to include in this instantiation of the suite          

        )pbdoc")
        .def(py::init<std::vector<int>, std::vector<int>, std::vector<int>>(), py::arg("problem_ids"),
             py::arg("instances"), py::arg("dimensions"));
}
