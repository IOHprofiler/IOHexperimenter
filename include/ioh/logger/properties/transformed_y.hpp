// filepath: /ioh-logger/ioh-logger/include/ioh/logger/properties/transformed_y.hpp
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
         * This property retrieves the transformed objective value (`transformed_y`) from the logging information.
         * It provides an interface to access this value for both single-objective and multi-objective problems.
         *
         * @ingroup Logging
         */
        struct TransformedY : public logger::Property
        {
            //! Constructor.
            /**
             * Initializes the `TransformedY` property with a default name and format.
             *
             * @param name The name of the property. Defaults to "transformed_y".
             * @param format The format string for the property. Defaults to `logger::DEFAULT_DOUBLE_FORMAT`.
             */
            TransformedY(const std::string name = "transformed_y", const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                Property(name, format)
            {
            }

            //! Operator overload for single-objective problems.
            /**
             * Retrieves the transformed objective value (`transformed_y`) from the logging information.
             *
             * @param log_info The logging information for a single-objective problem.
             * @return An optional containing the transformed objective value, or an empty optional if unavailable.
             */
            std::optional<double> operator()(const logger::Info<problem::SingleObjective> &log_info) const override
            {
                return std::make_optional(log_info.transformed_y);
            }

            //! Operator overload for multi-objective problems.
            /**
             * This property is not applicable for multi-objective problems and always returns an empty optional.
             *
             * @param log_info The logging information for a multi-objective problem.
             * @return An empty optional.
             */
            std::optional<double> operator()(const logger::Info<problem::MultiObjective> &) const override
            {
                return {};
            }
        };
        
        /** A property that accesses the best value found so far, with transformation.
         *
         * This property retrieves the transformed best objective function value (`transformed_y_best`) from the logging information.
         * It provides an interface to access this value for both single-objective and multi-objective problems.
         *
         * @ingroup Logging
         */
        inline TransformedY transformed_y; // Instance of the TransformedY property for logging.
        
    } // namespace logger
} // namespace ioh