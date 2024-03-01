#pragma once

#include <fmt/ranges.h>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <utility>
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