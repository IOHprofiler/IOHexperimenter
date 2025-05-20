// filepath: /ioh-logger/ioh-logger/include/ioh/logger/properties/raw_y_best.hpp
#pragma once

#include <optional>
#include <string>
#include "property.hpp"
#include "../loginfo.hpp"

namespace ioh
{
    namespace watch
    {
        /** A property that accesses the best value so far, without transformation.
         *
         * This property provides access to the raw best objective function value 
         * encountered during the optimization process. It is not subject to any 
         * transformations or scaling.
         *
         * @ingroup Logging
         */
        struct RawYBest : public logger::Property
        {
            /** Constructor.
             *
             * Initializes the property with a default name and format.
             *
             * @param name The name of the property (default: "raw_y_best").
             * @param format The format string for logging (default: DEFAULT_DOUBLE_FORMAT).
             */
            RawYBest(const std::string name = "raw_y_best", const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                Property(name, format)
            {
            }

            /** Main call interface for single-objective problems.
             *
             * Retrieves the raw best objective function value for single-objective problems.
             *
             * @param log_info The logging information containing the raw best value.
             * @return An optional containing the raw best value, if available.
             */
            std::optional<double> operator()(const logger::Info<problem::SingleObjective> &log_info) const override
            {
                return std::make_optional(log_info.raw_y_best);
            }

            /** Main call interface for multi-objective problems.
             *
             * This property is not applicable to multi-objective problems and always returns an empty optional.
             *
             * @param log_info The logging information for multi-objective problems.
             * @return An empty optional.
             */
            std::optional<double> operator()(const logger::Info<problem::MultiObjective> &log_info) const override
            {
                return {};
            }
        };
        
        /** Best objective function value found so far, without transformation.
         *
         * This is an inline instance of the `RawYBest` property, providing convenient access 
         * to the raw best value without requiring explicit instantiation.
         *
         * @ingroup Properties
         */
        inline RawYBest raw_y_best; 
    } // namespace logger
} // namespace ioh