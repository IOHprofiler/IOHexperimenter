// filepath: /ioh-logger/ioh-logger/include/ioh/logger/properties/raw_y.hpp
#pragma once

#include <optional>
#include <string>
#include "property.hpp"
#include "../loginfo.hpp"

namespace ioh
{
    namespace watch
    {
        /** A property that accesses the current objective value, without transformation.
         *
         * This property retrieves the raw objective value (`raw_y`) from the logging information.
         * If the value is infinite, it returns an empty optional.
         *
         * @ingroup Logging
         */
        struct RawY : public logger::Property
        {
            /** Constructor.
             *
             * Initializes the property with a name and format.
             *
             * @param name The name of the property (default: "raw_y").
             * @param format The format string for the property (default: DEFAULT_DOUBLE_FORMAT).
             */
            RawY(const std::string name = "raw_y", const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                Property(name, format)
            {
            }

            /** Main call interface for single-objective problems.
             *
             * Retrieves the raw objective value (`raw_y`) from the logging information.
             * If the value is infinite, an empty optional is returned.
             *
             * @param log_info The logging information for a single-objective problem.
             * @return An optional containing the raw objective value, or an empty optional if the value is infinite.
             */
            std::optional<double> operator()(const logger::Info<problem::SingleObjective> &log_info) const override
            {
                if (abs(log_info.raw_y) == std::numeric_limits<double>::infinity())
                {
                    return {};
                }
                return std::make_optional(log_info.raw_y);
            }

            /** Main call interface for multi-objective problems.
             *
             * This implementation does not provide a raw objective value for multi-objective problems
             * and always returns an empty optional.
             *
             * @param log_info The logging information for a multi-objective problem.
             * @return An empty optional.
             */
            std::optional<double> operator()(const logger::Info<problem::MultiObjective> &) const override
            {
                return {};
            }
        };

        /** Current function value, without transformation.
         *
         * @ingroup Properties
         */
        inline RawY raw_y; // Instance of the RawY property for logging.
    } // namespace logger
} // namespace ioh