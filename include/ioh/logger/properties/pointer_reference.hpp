#pragma once
#include <optional>
#include <string>
#include "property.hpp"
#include "../loginfo.hpp"

namespace ioh
{
    namespace watch
    {
        
        /** A property that accesses the variable of a variable through a reference to a pointer.
        *
        * Use this if the variable does not always exist in the logged scope.
        *
        * @warning It is your responsibility to ensure that the referenced pointer is a nullptr if the variable is out
        * of the logged scope.
        *
        * @note The pointed variable must be castable to a double.
        *
        * @ingroup Logging
        */
        template <class T>
        class PointerReference : public logger::Property
        {
        public:
            //! Typedef for the ptr
            using PtrType = T*;
            //! Typedef for the const ptr
            using ConstPtrType = PtrType const;
            //! Typedef for the const ptr ref
            using RefType = ConstPtrType&;
            //! Typedef for the const const ptr ref
            using ConstRefType = RefType;

        protected:
            //! The managed reference to a pointer.
            ConstRefType _ref_ptr_var;

#ifndef NDEBUG
        public:
            //! Accessor for the internal ptr
            RefType ref_ptr_var() const { return const_cast<RefType>(_ref_ptr_var); }
#endif
        public:
            /** Constructor.
                *
                * @param name the name of the property.
                * @param ref_ptr_var a reference to a pointer to the logged variable.
                * @param format a fmt::format specification
                */
            PointerReference(const std::string& name, ConstRefType ref_ptr_var, const std::string& format = logger::DEFAULT_DOUBLE_FORMAT)
                : logger::Property(name, format), _ref_ptr_var(ref_ptr_var)
            {
            }

            //! Main call interface.
            std::optional<double> operator()(const logger::Info<problem::SingleObjective>&) const override
            {
                if (_ref_ptr_var != nullptr)
                {
                    return std::make_optional(static_cast<double>(*_ref_ptr_var));
                }
                return std::nullopt;
            }

            std::optional<double> operator()(const logger::Info<problem::MultiObjective>&) const override
            {
                if (_ref_ptr_var != nullptr)
                {
                    return std::make_optional(static_cast<double>(*_ref_ptr_var));
                }
                return std::nullopt;
            }
        };

        /** The value of an external variable, which may not exist.
            *
            * Useful for variables that don't exist at some point during the solver run.
            * In that case, if the referenced pointer is a `std::nullptr`,
            * the value will be indicated as invalid in the logs.
            *
            * To do so, this captures a reference to a pointer toward your variable.
            * If you update the value of the pointed variables, it will change in the logs.
            * If you need to invalidate the variable, you can set the referenced pointer itself to `nullptr`.
            *
            * @warning It is your responsibility to ensure that the referenced pointer is a nullptr if the variable is out
            * of the logged scope.
            *
            * @param name the name of the property.
            * @param variable a reference to a pointer to the logged variable.
            * @param format a fmt::format specification
            *
            * @ingroup Properties
            */
        template <class T>
        PointerReference<T>& pointer(const std::string& name, const T* const& variable,
                                        const std::string& format = logger::DEFAULT_DOUBLE_FORMAT)
        {
            auto p = new PointerReference<T>(name, variable, format);
            return *p;
        }
        
    } // namespace logger
} // namespace ioh