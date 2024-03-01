#include <pybind11/pybind11.h>

namespace py = pybind11;

void define_real_suite(py::module &m);
void define_integer_suite(py::module &m);

void define_suites(py::module &m)
{
    define_real_suite(m);
    define_integer_suite(m);
}
