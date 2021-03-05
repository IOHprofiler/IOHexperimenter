#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "ioh.hpp"

namespace py = pybind11;
using namespace ioh::suite;

template <typename SuiteType>
void define_base_class(py::module &m, const std::string &name)
{
    py::class_<SuiteType, std::shared_ptr<SuiteType>>(m, name.c_str(), py::buffer_protocol())
        .def("reset", &SuiteType::reset)
        .def("attach_logger", &SuiteType::attach_logger)
        .def("detach_logger", &SuiteType::detach_logger)
        .def_property_readonly("problem_ids", &SuiteType::problem_ids)
        .def_property_readonly("dimensions", &SuiteType::dimensions)
        .def_property_readonly("instances", &SuiteType::problem_ids)
        .def_property_readonly("name", &SuiteType::problem_ids)
        .def("__len__", &SuiteType::size)
        .def("__iter__", [](SuiteType &s)
             {
                 return py::make_iterator(s.begin(), s.end());
            },
            py::keep_alive<0, 1>())
        ;
}


void define_suite(py::module &m)
{
    define_base_class<Suite<ioh::problem::Real>>(m, "RealBase");

    py::class_<Real, Suite<ioh::problem::Real>, std::shared_ptr<Real>>(m, "Real")
        .def(py::init<std::vector<int>, std::vector<int>, std::vector<int>>());
    
    py::class_<BBOB, Suite<ioh::problem::Real>, std::shared_ptr<BBOB>>(m, "BBOB")
        .def(py::init<std::vector<int>, std::vector<int>, std::vector<int>>());
    
    define_base_class<Suite<ioh::problem::Integer>>(m, "IntegerBase");
    
    py::class_<Integer, Suite<ioh::problem::Integer>, std::shared_ptr<Integer>>(m, "Integer")
        .def(py::init<std::vector<int>, std::vector<int>, std::vector<int>>());
    
    py::class_<PBO, Suite<ioh::problem::Integer>, std::shared_ptr<PBO>>(m, "PBO")
        .def(py::init<std::vector<int>, std::vector<int>, std::vector<int>>());

}
