#pragma once

// What every binding source of the Python module includes: nanobind with the casters it uses, the library, the array
// helpers, and the functions that the sources define for each other. The sources have no file-scope `using
// namespace`, and keep their own helpers in a namespace named after the file (ioh_python::<file>), so that they
// can also be compiled as one unity build.

#include <algorithm>
#include <string>
#include <vector>

#include <nanobind/make_iterator.h>
#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/filesystem.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/vector.h>
#include <nanobind/trampoline.h>

#include "ioh.hpp"

namespace nb = nanobind;

namespace ioh_python
{
    //! A one-dimensional input array (numpy or anything else with the buffer or DLPack protocol). Methods that take a
    //! vector define an overload with this type before the one with std::vector, so that a numpy array is copied at
    //! once instead of element by element, and lists keep working.
    template <typename T>
    using Array1D = nb::ndarray<const T, nb::ndim<1>, nb::c_contig, nb::device::cpu>;

    //! A two-dimensional input array, one solution per row
    template <typename T>
    using Array2D = nb::ndarray<const T, nb::ndim<2>, nb::c_contig, nb::device::cpu>;

    //! The values of a one-dimensional input array as a vector of T
    template <typename T, typename A>
    std::vector<T> to_vector(const A &array)
    {
        const auto *data = array.data();
        return std::vector<T>(data, data + array.shape(0));
    }

    //! The rows of a two-dimensional input array as vectors of T
    template <typename T, typename A>
    std::vector<std::vector<T>> to_vectors(const A &array)
    {
        std::vector<std::vector<T>> rows(array.shape(0));
        const auto *data = array.data();
        const size_t n = array.shape(1);
        for (size_t i = 0; i < rows.size(); i++)
            rows[i].assign(data + i * n, data + (i + 1) * n);
        return rows;
    }

    //! A numpy array that is a writable view of v, which owner keeps alive (the Python object that holds v, e.g. a
    //! Solution)
    template <typename T>
    nb::ndarray<nb::numpy, T, nb::ndim<1>> make_mutable_array(std::vector<T> &v, nb::handle owner)
    {
        return nb::ndarray<nb::numpy, T, nb::ndim<1>>(v.data(), {v.size()}, owner);
    }

    //! A numpy array with a copy of v
    template <typename T>
    nb::ndarray<nb::numpy, T, nb::ndim<1>> make_array(const std::vector<T> &v)
    {
        auto *data = new T[v.size()];
        std::copy(v.begin(), v.end(), data);
        nb::capsule owner(data, [](void *p) noexcept { delete[] static_cast<T *>(p); });
        return nb::ndarray<nb::numpy, T, nb::ndim<1>>(data, {v.size()}, owner);
    }
} // namespace ioh_python

// The parts of the module, defined by the binding sources (ioh.cpp calls the first four, in this order)

// problem_helpers.cpp: Solution, State, constraints, bounds, MetaData, log info, ...
void define_helper_classes(nb::module_ &m);
// logger.cpp: the logger module; it calls define_triggers (trigger.cpp), define_properties (property.cpp) and
// define_eah/define_eaf (empirical_attainment.cpp)
void define_logger(nb::module_ &m);
void define_triggers(nb::module_ &m);
void define_properties(nb::module_ &m);
void define_eah(nb::module_ &m);
void define_eaf(nb::module_ &m);
// problem.cpp: the problem base classes, and the problem families defined in real.cpp and integer.cpp
void define_problem(nb::module_ &m);
void define_bbob(nb::module_ &m);
void define_cec2013_problems(nb::module_ &m);
void define_cec2022_problems(nb::module_ &m);
void define_dynamic_bin_val_problem(nb::module_ &m);
void define_star_discrepancy_problems(nb::module_ &m);
void define_many_affine(nb::module_ &m);
void define_funnel(nb::module_ &m);
void define_pbo_problems(nb::module_ &m);
void define_wmodels(nb::module_ &m);
void define_submodular_problems(nb::module_ &m);
// suite.cpp: the suite module, from real_suite.cpp and integer_suite.cpp, which share suite_docstring
// (suite_helpers.cpp)
void define_suites(nb::module_ &m);
void define_real_suite(nb::module_ &m);
void define_integer_suite(nb::module_ &m);
std::string suite_docstring(const std::string &name);
