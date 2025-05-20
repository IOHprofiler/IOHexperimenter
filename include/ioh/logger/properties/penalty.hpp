
#pragma once

#include <optional>
#include "property.hpp"
#include "../loginfo.hpp"

namespace ioh
{
    namespace watch
    {
        /** A property that accesses the current constraint penalty
         *
         * This property retrieves the penalty value associated with a specific constraint
         * index from the logger information. It supports both single-objective and 
         * multi-objective problems.
         *
         * @ingroup Logging
         */
        struct Penalty : public logger::Property
        {
            /** Constructor for the Penalty property
             *
             * Initializes the Penalty property with a name, format, and constraint index.
             *
             * @param name The name of the property (default: "penalty").
             * @param format The format string for logging the penalty value (default: logger::DEFAULT_DOUBLE_FORMAT).
             * @param ci The index of the constraint to be logged (default: 0).
             */
            Penalty(const std::string name = "penalty", 
                    const std::string &format = logger::DEFAULT_DOUBLE_FORMAT,
                    const size_t ci = 0
                ) :
                logger::Property(name, format),
                ci(ci)
            {
            }

            /** Retrieve the penalty value for single-objective problems
             *
             * This operator fetches the penalty value for the specified constraint index
             * from the logger information of a single-objective problem.
             *
             * @param log_info The logger information for a single-objective problem.
             * @return An optional containing the penalty value for the specified constraint index.
             */
            std::optional<double> operator()(const logger::Info<problem::SingleObjective> &log_info) const override
            {
                return std::make_optional(log_info.penalties[ci]);
            }

            /** Retrieve the penalty value for multi-objective problems
             *
             * This operator fetches the penalty value for the specified constraint index
             * from the logger information of a multi-objective problem.
             *
             * @param log_info The logger information for a multi-objective problem.
             * @return An optional containing the penalty value for the specified constraint index.
             */
            std::optional<double> operator()(const logger::Info<problem::MultiObjective> &log_info) const override
            {
                return std::make_optional(log_info.penalties[ci]);
            }

        private:
            //! Index of the constraint to be logged
            size_t ci;
        };

        /** Global instance of the Penalty property
         *
         * This instance provides access to the penalty property for logging purposes.
         *
         * @ingroup Properties
         */
        inline Penalty penalty;
    } // namespace watch
} // namespace ioh