#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "ioh.hpp"

namespace py = pybind11;
using namespace ioh::suite;

std::string make_docstring(const std::string &name)
{
    return fmt::format(R"pbdoc(
        Suite with {} functions included in IOH. 

        This class is an iterator for problems, you can define a set of problems to iterate over.

        Parameters
        ----------
        problem_ids: list[int]
            A list of problems ids to include in this instantiation of the suite
        instances: list[int]
            A list of problem instances to include in this instantiation of the suite
        problem_ids: list[int]
            A list of problem dimensions to include in this instantiation of the suite          
    )pbdoc",
                       name);
}
