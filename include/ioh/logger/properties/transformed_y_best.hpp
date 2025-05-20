// filepath: /ioh-logger/ioh-logger/include/ioh/logger/properties/transformed_y_best.hpp
#pragma once

#include <optional>
#include <string>
#include "property.hpp"
#include "../loginfo.hpp"

namespace ioh
{
    namespace watch
    {
        /** A property that accesses the best value found so far, with transformation.
         *
         * This property retrieves the best objective function value found so far 
         * during the optimization process, applying any transformations defined 
         * by the problem. It provides an interface for both single-objective and 
         * multi-objective problems, though it only returns a value for single-objective 
         * problems.
         *
         * @ingroup Logging
         */
        struct TransformedYBest : public logger::Property
        {
            /** Constructor.
             *
             * Initializes the property with a default name ("transformed_y_best") 
             * and a format string for logging the value.
             *
             * @param name The name of the property (default: "transformed_y_best").
             * @param format The format string for logging the value (default: logger::DEFAULT_DOUBLE_FORMAT).
             */
            TransformedYBest(const std::string name = "transformed_y_best", const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                Property(name, format)
            {
            }

            /** Main call interface for single-objective problems.
             *
             * Retrieves the transformed best objective function value found so far 
             * for single-objective problems.
             *
             * @param log_info The logging information containing the transformed best value.
             * @return An optional containing the transformed best value, or an empty optional if not available.
             */
            std::optional<double> operator()(const logger::Info<problem::SingleObjective> &log_info) const override
            {
                return std::make_optional(log_info.transformed_y_best);
            }

            /** Main call interface for multi-objective problems.
             *
             * This property is not applicable to multi-objective problems and 
             * always returns an empty optional.
             *
             * @param log_info The logging information for multi-objective problems.
             * @return An empty optional.
             */
            std::optional<double> operator()(const logger::Info<problem::MultiObjective> &log_info) const override
            {
                return {};
            }
        };

        /** Best objective function value found so far, with transformation.
         *
         * A global instance of the `TransformedYBest` property, which can be used 
         * directly to access the transformed best value during logging.
         *
         * @ingroup Properties
         */
        inline TransformedYBest transformed_y_best; 
    } // namespace logger
} // namespace ioh