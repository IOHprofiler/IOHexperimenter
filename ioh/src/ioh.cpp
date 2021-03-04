#include <pybind11/pybind11.h> 

namespace py = pybind11;

void init_problem(py::module& m);
void init_suite(py::module& m);
void init_logger(py::module& m);
void init_experimenter(py::module& m);

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

    init_logger(m_logger);
    init_problem(m_problem);
    init_suite(m_suite);
    
    init_experimenter(m_experimenter);

    m.attr("__version__") = "dev";
}
