#pragma once

#include <optional>
#include <string>
#include "property.hpp"
#include "../loginfo.hpp"

namespace ioh
{
    namespace watch
    {
        /** A property that accesses a variable through a pointer.
         *
         * This class allows logging of a variable by accessing it through a pointer.
         * The variable must be castable to a `double` type.
         *
         * @tparam T The type of the variable being accessed.
         *
         * @ingroup Logging
         */
        template <class T>
        class Pointer : public logger::Property
        {
        protected:
            //! The managed variable.
            //! A constant pointer to the variable being logged.
            const T *const _variable;

        public:
            /** Constructor.
             *
             * Initializes the `Pointer` property with a name, a pointer to the variable, 
             * and an optional format specification.
             *
             * @param name The name of the property.
             * @param variable A pointer to the logged variable. Must not be `nullptr`.
             * @param format A `fmt::format` specification string. Defaults to `DEFAULT_DOUBLE_FORMAT`.
             */
            Pointer(const std::string name, const T *const variable, const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                Property(name, format), _variable(variable)
            {
                assert(variable != nullptr); // Ensure the pointer is not null.
            }

            /** Main call interface for single-objective problems.
             *
             * Retrieves the value of the pointed variable, cast to a `double`.
             *
             * @param info The logger information for a single-objective problem.
             * @return An optional containing the value of the variable as a `double`.
             */
            std::optional<double> operator()(const logger::Info<problem::SingleObjective> &info) const override
            {
                return std::make_optional(static_cast<double>(*_variable));
            }

            /** Main call interface for multi-objective problems.
             *
             * Retrieves the value of the pointed variable, cast to a `double`.
             *
             * @param info The logger information for a multi-objective problem.
             * @return An optional containing the value of the variable as a `double`.
             */
            std::optional<double> operator()(const logger::Info<problem::MultiObjective> &info) const override
            {
                return std::make_optional(static_cast<double>(*_variable));
            }

        };
        /** The value of an extern variable (captured by address).
         *
         * @param name the name of the property.
         * @param variable a pointer to the logged variable.
         * @param format a string to format the variable when logging. 
         *
         * @ingroup Properties
         */
         template <class T>
         Pointer<T> &address(const std::string name, const T *const variable, const std::string &format = logger::DEFAULT_DOUBLE_FORMAT)
         {
             auto p = new Pointer<T>(name, variable, format);
             return *p;
         }
    } // namespace logger
} // namespace ioh