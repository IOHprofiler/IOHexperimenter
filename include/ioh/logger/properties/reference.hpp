#pragma once

#include <optional>
#include <string>
#include "property.hpp"
#include "../loginfo.hpp"

namespace ioh
{
    namespace watch
    {
        /** A property that accesses a referenced variable.
         *
         * This class allows logging of a variable by maintaining a reference to it.
         * The variable must be castable to a `double` for proper functionality.
         *
         * @tparam T The type of the referenced variable.
         * @ingroup Logging
         */
        template <class T>
        class Reference : public logger::Property
        {
        protected:
            //! The managed reference to the variable being logged.
            const T &_variable;

        public:
            /** Constructor.
             *
             * Initializes the `Reference` property with a name, a reference to the variable, 
             * and an optional format string.
             *
             * @param name The name of the property.
             * @param variable A reference to the variable to be logged.
             * @param format A `fmt::format` specification string (default is `DEFAULT_DOUBLE_FORMAT`).
             */
            Reference(const std::string name, const T &variable, const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                Property(name, format), _variable(variable)
            {
            }

            /** Main call interface for single-objective problems.
             *
             * This function is called to retrieve the value of the referenced variable
             * for logging purposes. The variable is cast to a `double` and returned as an optional.
             *
             * @param info The logging information for a single-objective problem.
             * @return An optional containing the casted value of the referenced variable.
             */
            std::optional<double> operator()(const logger::Info<problem::SingleObjective> &) const override
            {
                return std::make_optional(static_cast<double>(_variable));
            }

            /** Main call interface for multi-objective problems.
             *
             * This function is called to retrieve the value of the referenced variable
             * for logging purposes. The variable is cast to a `double` and returned as an optional.
             *
             * @param info The logging information for a multi-objective problem.
             * @return An optional containing the casted value of the referenced variable.
             */
            std::optional<double> operator()(const logger::Info<problem::MultiObjective> &) const override
            {
                return std::make_optional(static_cast<double>(_variable));
            }
        };
        /** The value of an extern variable (captured by reference).
        *
        * @param name the name of the property.
        * @param variable a reference to the logged variable.
        * @param format a fmt::format specification
        *
        * @ingroup Properties
        */
        template <class T>
        Reference<T> &reference(const std::string name, const T &variable, const std::string &format = logger::DEFAULT_DOUBLE_FORMAT)
        {
            auto p = new Reference<T>(name, variable, format);
            return *p;
        }
    } // namespace logger
} // namespace ioh