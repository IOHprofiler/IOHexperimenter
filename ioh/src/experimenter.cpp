#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "ioh.hpp"

namespace py = pybind11;
using namespace ioh::experiment;

template <typename ProblemType>
void define_experimenter(py::module &m, const std::string &name)
{
    using Experimenter = Experimenter<ProblemType>;
    using Suite = std::shared_ptr<ioh::suite::Suite<ProblemType>>;
    py::class_<Experimenter>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<Suite, std::shared_ptr<ioh::logger::Base>, typename Experimenter::Algorithm, int>())
        .def(py::init<fs::path, typename Experimenter::Algorithm>())
        .def("run", &Experimenter::run)
        .def_property("independent_runs",
                      py::overload_cast<void>(&Experimenter::independent_runs, py::const_),
                      py::overload_cast<const int>(&Experimenter::independent_runs))
        .def("suite", &Experimenter::suite)
        .def("logger", &Experimenter::logger);
}

void define_experimenter(py::module &m)
{
    define_experimenter<ioh::problem::Real>(m, "Real");
    define_experimenter<ioh::problem::Integer>(m, "Integer");
}
