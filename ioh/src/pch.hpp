#pragma once

#include <utility>

#include <fmt/ranges.h>
#include <nanobind/make_iterator.h>
#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/filesystem.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/trampoline.h>

#include "ioh.hpp"

namespace nb = nanobind;

using namespace ioh;

// Python spec. implementation
class PyProperty : public ioh::logger::Property
{
    const nb::object container_;
    const std::string attribute_;

public:
    PyProperty(const nb::object &container, const std::string &attribute) :
        Property(attribute), container_(container), attribute_(attribute)
    {
    }

    nb::object container() const { return container_; }

    std::optional<double> operator()(const logger::Info &) const override
    {
        if (nb::hasattr(container_, attribute_.c_str()))
        {
            auto pyobj = container_.attr(attribute_.c_str()).ptr();

            if (pyobj != Py_None)
                return std::make_optional<double>(PyFloat_AsDouble(pyobj));
        }
        return {};
    }
};

template <typename T>
using Array1D = nb::ndarray<nb::numpy, T, nb::ndim<1>, nb::device::cpu, nb::c_contig>;

template <typename T>
Array1D<T> make_array(const std::vector<T> &x)
{
    double *data = new double[x.size()];
    for (size_t i = 0; i < x.size(); i++)
        data[i] = x[i];

    nb::capsule owner(data, [](void *p) noexcept { delete[] (double *)p; });

    return Array1D<T>(data, {x.size()}, owner);
}

template <typename T>
Array1D<T> make_mutable_array(std::vector<T> &v, nb::object owner)
{
    // nb::gil_scoped_acquire();
    // return Array1D<T>(
    //     v.data(),
    //     {v.size()},
    //     owner
    // ).cast();
    return make_array(v);
}