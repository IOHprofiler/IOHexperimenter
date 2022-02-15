#include <pybind11/pybind11.h>
#include "ioh.hpp"

namespace py = pybind11;

void define_helper_classes(py::module& m);
void define_problem(py::module& m);
void define_suite(py::module& m);
void define_logger(py::module& m);

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

    auto m_problem = m.def_submodule("problem", "Module that includes all IOHExperimenter problems");
    auto m_suite = m.def_submodule("suite", "Module that includes all IOHExperimenter suites");
    auto m_logger = m.def_submodule("logger", "Module that includes all IOHExperimenter loggers");
    
    define_helper_classes(m);
    define_logger(m_logger);
    define_problem(m_problem);
    define_suite(m_suite);

    m.attr("__version__") = "dev";
}
