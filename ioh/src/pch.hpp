#pragma once

#include <utility>

#include <fmt/ranges.h>

#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "ioh.hpp"


namespace py = pybind11;
using namespace ioh;

// Python spec. implementation
class PyProperty : public ioh::logger::Property 
{
    const py::object container_;
    const std::string attribute_;

public:
    PyProperty(const py::object &container, const std::string &attribute) :
        Property(attribute), container_(container), attribute_(attribute)
    {
    }

    py::object container() const { return container_; }

    std::optional<double> operator()(const logger::Info &) const override
    {
        if (py::hasattr(container_, attribute_.c_str()))
        {
            auto pyobj = container_.attr(attribute_.c_str()).ptr();

            if (pyobj != Py_None)
                return std::make_optional<double>(PyFloat_AsDouble(pyobj));
        }
        return {};
    }
};

template<typename T>
py::array_t<T> make_mutable_array(std::vector<T>& v, py::object owner)
{
    return py::array_t<T>(
        {v.size()},      // shape
        {sizeof(T)},     // stride
        v.data(),        // pointer
        owner            // keep the parent (e.g. Solution) alive
    );
}


template<typename T>
py::array_t<T> make_array(const std::vector<T>& x)
{
    py::array_t<T> arr(
        {static_cast<size_t>(x.size())}, 
        {static_cast<size_t>(sizeof(T))},
        static_cast<const T*>(x.data())
    );
    return arr;
}