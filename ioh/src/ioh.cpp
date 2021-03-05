#include <pybind11/pybind11.h> 
#include "ioh.hpp"

namespace py = pybind11;

void define_helper_classes(py::module& m);
void define_problem(py::module& m);
void define_suite(py::module& m);
void define_logger(py::module& m);
void define_experimenter(py::module& m);

PYBIND11_MAKE_OPAQUE(std::vector<double>);
PYBIND11_MAKE_OPAQUE(std::vector<float>);
PYBIND11_MAKE_OPAQUE(std::vector<int>);

PYBIND11_MODULE(iohcpp, m) {
    m.doc() = R"pbdoc(
        IOHExperimenter Python Interface
    )pbdoc";

    auto m_problem = m.def_submodule("problem", "IOHExperimenter problems");
    auto m_suite = m.def_submodule("suite", "IOHExperimenter suites");
    auto m_logger = m.def_submodule("logger", "IOHExperimenter logger");
    auto m_experimenter = m.def_submodule("experimenter", "IOHExperimenter experimenter");

    define_helper_classes(m);
    define_logger(m_logger);
    define_problem(m_problem);
    define_suite(m_suite);
    define_experimenter(m_experimenter);

    m.attr("__version__") = "dev";
}
