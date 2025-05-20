#pragma once

#include <optional>
#include <string>
#include "property.hpp"
#include "../loginfo.hpp"

namespace ioh
{
    namespace watch
    {
        /** A property that accesses the current constraint violation.
         *
         * This property retrieves the value of a specific constraint violation
         * from the logger information. It is used for logging purposes in the
         * context of optimization problems.
         *
         * @ingroup Logging
         */
        struct Violation : public logger::Property
        {
            /** Constructor.
             *
             * Initializes the `Violation` property with a name, format, and
             * the index of the constraint to be logged.
             *
             * @param name The name of the property (default: "violation").
             * @param format The format string for logging (default: logger::DEFAULT_DOUBLE_FORMAT).
             * @param ci The index of the constraint to be logged (default: 0).
             */
            Violation(const std::string name = "violation",
                      const std::string &format = logger::DEFAULT_DOUBLE_FORMAT,
                      const size_t ci = 0)
                : logger::Property(name, format), ci(ci)
            {
            }

            /** Retrieve the constraint violation for a single-objective problem.
             *
             * @param log_info The logger information containing the constraint violations.
             * @return An optional double representing the value of the constraint violation.
             */
            std::optional<double> operator()(const logger::Info<problem::SingleObjective> &log_info) const override
            {
                return std::make_optional(log_info.violations[ci]);
            }

            /** Retrieve the constraint violation for a multi-objective problem.
             *
             * @param log_info The logger information containing the constraint violations.
             * @return An optional double representing the value of the constraint violation.
             */
            std::optional<double> operator()(const logger::Info<problem::MultiObjective> &log_info) const override
            {
                return std::make_optional(log_info.violations[ci]);
            }

        private:
            //! Index of the constraint to be logged.
            size_t ci;
        };

        /** A global instance of the `Violation` property.
         *
         * This instance can be used to log the constraint violation values
         * without needing to explicitly create a new `Violation` object.
         *
         * @ingroup Properties
         */
        inline Violation violation;
    } // namespace watch
} // namespace ioh