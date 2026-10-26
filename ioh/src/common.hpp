#pragma once

// What every binding source of the Python module includes: nanobind with the casters it uses, the library, the array
// helpers, and the functions that the sources define for each other. The sources have no file-scope `using
// namespace`, and keep their own helpers in a namespace named after the file (ioh_python::<file>), so that they
// can also be compiled as one unity build.

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <limits>
#include <type_traits>
#include <utility>
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
    //! An input array argument (Array1D and the others below): a nanobind ndarray whose caster turns lists and
    //! tuples down at once. nanobind's own caster never accepts them either, but under the limited API (the abi3
    //! wheel, TASKS.md T6.7) it first looks up __dlpack__ through an AttributeError, which made every call with a
    //! list about twice as slow; the overload with std::vector then takes the list.
    template <typename A>
    struct InputArray : A
    {
        InputArray() = default;
        explicit InputArray(A &&array) : A(std::move(array)) {}
    };
} // namespace ioh_python

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

//! The caster of an InputArray: the ndarray caster (with its name in signatures and stubs), once the argument is not a
//! list or tuple
template <typename A>
struct type_caster<ioh_python::InputArray<A>>
{
    using Caster = make_caster<A>;
    NB_TYPE_CASTER(ioh_python::InputArray<A>, Caster::Name)

    bool from_python(handle src, uint32_t flags, cleanup_list *cleanup) noexcept
    {
        if (PyList_CheckExact(src.ptr()) || PyTuple_CheckExact(src.ptr()))
            return false;
        Caster caster;
        if (!caster.from_python(src, flags, cleanup))
            return false;
        value = ioh_python::InputArray<A>(std::move(caster.value));
        return true;
    }

    static handle from_cpp(const ioh_python::InputArray<A> &array, rv_policy policy, cleanup_list *cleanup) noexcept
    {
        return Caster::from_cpp(array, policy, cleanup);
    }
};

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)

namespace ioh_python
{
    //! A one-dimensional input array (numpy or anything else with the buffer or DLPack protocol). Methods that take a
    //! vector define an overload with this type before the one with std::vector, so that a numpy array is copied at
    //! once instead of element by element, and lists keep working.
    template <typename T>
    using Array1D = InputArray<nb::ndarray<const T, nb::ndim<1>, nb::c_contig, nb::device::cpu>>;

    //! A two-dimensional input array, one solution per row
    template <typename T>
    using Array2D = InputArray<nb::ndarray<const T, nb::ndim<2>, nb::c_contig, nb::device::cpu>>;

    //! An integer input array: one or two-dimensional, of any integer or bool type (e.g. a bitstring as
    //! `rng.random(n) < 0.5`). Methods that take integers take it with array_arg<int>, so that it is never converted:
    //! an array of floats raises TypeError (to_vector), as pybind11 did.
    using IntArray1D = InputArray<nb::ndarray<nb::ndim<1>, nb::c_contig, nb::device::cpu>>;
    using IntArray2D = InputArray<nb::ndarray<nb::ndim<2>, nb::c_contig, nb::device::cpu>>;

    //! The input array of a method over T: Array1D<T> for reals, IntArray1D for integers
    template <typename T>
    using InputArray1D = std::conditional_t<std::is_same_v<T, int>, IntArray1D, Array1D<T>>;
    template <typename T>
    using InputArray2D = std::conditional_t<std::is_same_v<T, int>, IntArray2D, Array2D<T>>;

    //! The argument of an input array over T. Integer arrays are taken only as they are (no conversion pass).
    template <typename T>
    constexpr auto array_arg(const char *name = nullptr)
    {
        if constexpr (std::is_integral_v<T>)
            return nb::arg(name).noconvert();
        else
            return nb::arg(name);
    }

    //! Raises TypeError, as pybind11 did, if the values of an integer array do not fit into T
    template <typename T, typename V>
    void check_range(const V *data, const size_t size)
    {
        if constexpr (std::is_integral_v<T> && std::is_integral_v<V> && !std::is_same_v<V, bool> &&
                      (sizeof(V) > sizeof(T) || (std::is_unsigned_v<V> && sizeof(V) == sizeof(T))))
            for (size_t i = 0; i < size; i++)
            {
                bool fits;
                if constexpr (std::is_signed_v<V>)
                    fits = data[i] >= static_cast<V>(std::numeric_limits<T>::min()) &&
                        data[i] <= static_cast<V>(std::numeric_limits<T>::max());
                else
                    fits = data[i] <= static_cast<std::make_unsigned_t<T>>(std::numeric_limits<T>::max());
                if (!fits)
                    throw nb::type_error("an integer in the array is out of range");
            }
    }

    //! Copies n values of type V at data to out, as T
    template <typename T, typename V>
    void append_values(std::vector<T> &out, const void *data, const size_t n)
    {
        const auto *values = static_cast<const V *>(data);
        check_range<T>(values, n);
        out.insert(out.end(), values, values + n);
    }

    //! Copies n values of an integer or bool array of type dtype to out, as ints; other arrays raise TypeError
    inline void append_ints(std::vector<int> &out, const void *data, const size_t n, const nb::dlpack::dtype dtype)
    {
        using code = nb::dlpack::dtype_code;
        const auto c = static_cast<code>(dtype.code);
        if (c == code::Bool && dtype.bits == 8)
            return append_values<int, bool>(out, data, n);
        if (c == code::Int)
            switch (dtype.bits)
            {
            case 8:
                return append_values<int, int8_t>(out, data, n);
            case 16:
                return append_values<int, int16_t>(out, data, n);
            case 32:
                return append_values<int, int32_t>(out, data, n);
            case 64:
                return append_values<int, int64_t>(out, data, n);
            default:
                break;
            }
        if (c == code::UInt)
            switch (dtype.bits)
            {
            case 8:
                return append_values<int, uint8_t>(out, data, n);
            case 16:
                return append_values<int, uint16_t>(out, data, n);
            case 32:
                return append_values<int, uint32_t>(out, data, n);
            case 64:
                return append_values<int, uint64_t>(out, data, n);
            default:
                break;
            }
        throw nb::type_error("expected an array of integers or bools");
    }

    //! The values of a one-dimensional input array as a vector of T
    template <typename T, typename A>
    std::vector<T> to_vector(const A &array)
    {
        if constexpr (std::is_same_v<A, IntArray1D>)
        {
            static_assert(std::is_same_v<T, int>, "an IntArray1D gives ints");
            std::vector<int> values;
            values.reserve(array.shape(0));
            append_ints(values, array.data(), array.shape(0), array.dtype());
            return values;
        }
        else
        {
            const auto *data = array.data();
            check_range<T>(data, array.shape(0));
            return std::vector<T>(data, data + array.shape(0));
        }
    }

    //! The rows of a two-dimensional input array as vectors of T
    template <typename T, typename A>
    std::vector<std::vector<T>> to_vectors(const A &array)
    {
        std::vector<std::vector<T>> rows(array.shape(0));
        const size_t n = array.shape(1);
        if constexpr (std::is_same_v<A, IntArray2D>)
        {
            static_assert(std::is_same_v<T, int>, "an IntArray2D gives ints");
            const auto *data = static_cast<const char *>(array.data());
            const size_t row_bytes = n * array.dtype().bits / 8;
            for (size_t i = 0; i < rows.size(); i++)
            {
                rows[i].reserve(n);
                append_ints(rows[i], data + i * row_bytes, n, array.dtype());
            }
        }
        else
        {
            const auto *data = array.data();
            check_range<T>(data, rows.size() * n);
            for (size_t i = 0; i < rows.size(); i++)
                rows[i].assign(data + i * n, data + (i + 1) * n);
        }
        return rows;
    }

    //! What a Python function returned, as T. A value that is not one raises RuntimeError naming its type (nanobind's
    //! own message is only std::bad_cast).
    template <typename T>
    T cast_result(const nb::handle value, const char *function)
    {
        T result{};
        if (!nb::try_cast<T>(value, result))
            throw std::runtime_error(std::string(function) + " returned " + nb::type_name(value.type()).c_str() +
                                     ", which cannot be used as its result");
        return result;
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

// problem.cpp: keeps a Python function alive for the life of the process (wrapped functions, constraints)
bool register_python_fn(nb::handle f);

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
