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
        Python Interface to IOHexperimenter, which provides

            * A framework for straightforward benchmarking of any iterative optimization heuristic
            * A suite consisting of 23 pre-made Pseudo-Boolean benchmarking function, with easily
               accessible methods for adding custom functions and suites
            * Logging methods to effortlesly store benchmarking data in a format compatible with
              IOHanalyzer, with future support for additional data logging options
        It is one part of the IOHprofiler project (https://github.com/IOHprofiler)

        Examples
        --------
        ```
        > from IOHexperimenter import IOH_function, IOH_logger
        > f = IOH_function(fid = fid, dim = 16, iid = 1, suite = 'BBOB')
        > logger = IOH_logger(
        >     location = "./data", foldername = "data",
        >     name = "some algorithm to test",
        >     info = "test of IOHexperimenter in python"
        > )
        > f.add_logger(logger)
        > f([1, 2, 3])
        ```

        Contact us
        ----------
        If you have any questions, comments or suggestions, please don't hesitate contacting
        us via IOHprofiler@liacs.leidenuniv.nl

        Cite us
        -------
        @ARTICLE{IOHprofiler,
            author = {Carola Doerr and Hao Wang and Furong Ye and Sander van Rijn and Thomas B{\"a}ck},
            title = {{IOHprofiler: A Benchmarking and Profiling Tool for Iterative Optimization Heuristics}},
            journal = {arXiv e-prints:1810.05281},
            archivePrefix = "arXiv",
            eprint = {1810.05281},
            year = 2018,
            month = oct,
            keywords = {Computer Science - Neural and Evolutionary Computing},
            url = {https://arxiv.org/abs/1810.05281}
        }
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
