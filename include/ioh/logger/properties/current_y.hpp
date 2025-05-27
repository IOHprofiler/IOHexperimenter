// filepath: /ioh-logger/ioh-logger/include/ioh/logger/properties/current_y.hpp
#pragma once

#include <optional>
#include <string>
#include "property.hpp"
#include "../loginfo.hpp"

namespace ioh
{
    namespace watch
    {
        /** A property that accesses the current value so far, with transformation.
         *
         * This property retrieves the current value (`y`) from the logging information
         * for single-objective problems. For multi-objective problems, it returns an empty optional.
         *
         * @ingroup Logging
         */
        struct CurrentY : public logger::Property
        {
            /** Constructor.
             *
             * Initializes the property with a default name ("current_y") and a format string.
             *
             * @param name The name of the property (default: "current_y").
             * @param format The format string for the property (default: logger::DEFAULT_DOUBLE_FORMAT).
             */
            CurrentY(const std::string name = "current_y", const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                Property(name, format)
            {
            }

            /** Operator to retrieve the current value for single-objective problems.
             *
             * @param log_info The logging information containing the current value.
             * @return An optional containing the current value (`y`) from the log info.
             */
            std::optional<double> operator()(const logger::Info<problem::SingleObjective> &log_info) const override
            {
                return std::make_optional(log_info.y);
            }

            /** Operator to retrieve the current value for multi-objective problems.
             *
             * This implementation always returns an empty optional, as the current value
             * is not applicable for multi-objective problems.
             *
             * @param log_info The logging information (unused in this implementation).
             * @return An empty optional.
             */
            std::optional<double> operator()(const logger::Info<problem::MultiObjective> &) const override
            {
                return {};
            }
        };
        /** A property that accesses the current best value so far, with transformation.
         *
         * This property retrieves the current best value (`y_best`) from the logging information
         * for single-objective problems. For multi-objective problems, it returns an empty optional.
         *
         * @ingroup Logging
         */
        inline CurrentY  current_y; 
    } // namespace logger
} // namespace ioh