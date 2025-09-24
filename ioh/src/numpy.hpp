#pragma once

#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;


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