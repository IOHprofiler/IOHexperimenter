// filepath: /ioh-logger/ioh-logger/include/ioh/logger/properties/current_y_best.hpp
#pragma once

#include <optional>
#include <string>
#include "property.hpp"
#include "../loginfo.hpp"

namespace ioh
{
    namespace watch
    {
        /** A property that accesses the current best value so far, with transformation.
         *
         * This property retrieves the best objective function value encountered so far
         * during the optimization process. It supports both single-objective and 
         * multi-objective problems, but only provides meaningful data for single-objective 
         * problems. For multi-objective problems, it returns an empty optional.
         *
         * @ingroup Logging
         */
        struct CurrentBestY : public logger::Property
        {
            /** Constructor.
             *
             * Initializes the property with a default name ("current_y_best") and an optional
             * format string for formatting the output. The format string defaults to the 
             * `logger::DEFAULT_DOUBLE_FORMAT`.
             *
             * @param name The name of the property (default: "current_y_best").
             * @param format The format string for output (default: logger::DEFAULT_DOUBLE_FORMAT).
             */
            CurrentBestY(const std::string name = "current_y_best",
                         const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                Property(name, format)
            {
            }

            /** Operator to retrieve the current best value for single-objective problems.
             *
             * This operator is called with logging information specific to single-objective
             * problems and returns the best objective function value encountered so far.
             *
             * @param log_info The logging information for a single-objective problem.
             * @return An optional containing the best objective function value, or an empty
             *         optional if no value is available.
             */
            std::optional<double> operator()(const logger::Info<problem::SingleObjective> &log_info) const override
            {
                return std::make_optional(log_info.y_best);
            }

            /** Operator to retrieve the current best value for multi-objective problems.
             *
             * This operator is called with logging information specific to multi-objective
             * problems. Since this property is not applicable to multi-objective problems,
             * it always returns an empty optional.
             *
             * @param log_info The logging information for a multi-objective problem.
             * @return An empty optional, as this property is not applicable to multi-objective problems.
             */
            std::optional<double> operator()(const logger::Info<problem::MultiObjective> &log_info) const override
            {
                return {};
            }
        };

        /** Best objective function value found so far, with transformation.
         *
         * This is an inline instance of the `CurrentBestY` property, initialized with
         * the default name and format. It can be used directly to access the current
         * best objective function value during logging.
         *
         * @ingroup Properties
         */
        inline CurrentBestY current_y_best; 
    } // namespace logger
} // namespace ioh