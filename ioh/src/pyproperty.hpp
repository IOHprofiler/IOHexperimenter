#pragma once

// What logger.cpp and property.cpp share: the property that reads an attribute of a Python object, and the type caster
// for std::reference_wrapper (loggers take their triggers and properties as vectors of references, and Combine its
// loggers), which nanobind does not have.

#include "common.hpp"

#include <fmt/ranges.h>
#include <functional>
#include <optional>
#include <utility>

namespace ioh_python
{
    //! A property that tracks the attribute named attribute of a Python object (container)
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

        std::optional<double> operator()(const ioh::logger::Info &) const override
        {
            if (nb::hasattr(container_, attribute_.c_str()))
            {
                auto pyobj = container_.attr(attribute_.c_str());

                if (!pyobj.is_none())
                    return std::make_optional<double>(PyFloat_AsDouble(pyobj.ptr()));
            }
            return {};
        }
    };
} // namespace ioh_python

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

//! std::reference_wrapper<T> of a bound type T: refers to the C++ object of the Python argument (as pybind11's caster),
//! and is returned to Python as a reference (no copy, no ownership)
template <typename T>
struct type_caster<std::reference_wrapper<T>>
{
    using Caster = make_caster<T>;
    static_assert(is_base_caster_v<Caster>, "std::reference_wrapper<T> needs a bound type T");

    using Value = std::reference_wrapper<T>;
    static constexpr auto Name = Caster::Name;
    template <typename T_>
    using Cast = Value;

    Caster caster;

    bool from_python(handle src, uint32_t flags, cleanup_list *cleanup) noexcept
    {
        return caster.from_python(src, flags_for_local_caster<T &>(flags), cleanup) &&
            caster.template can_cast<T &>();
    }

    static handle from_cpp(const Value &value, rv_policy policy, cleanup_list *cleanup) noexcept
    {
        if (policy == rv_policy::automatic || policy == rv_policy::automatic_reference ||
            policy == rv_policy::take_ownership)
            policy = rv_policy::reference;
        return Caster::from_cpp(&value.get(), policy, cleanup);
    }

    template <typename T_>
    bool can_cast() const noexcept
    {
        return true;
    }

    explicit operator Value() { return std::ref(caster.operator T &()); }
};

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
