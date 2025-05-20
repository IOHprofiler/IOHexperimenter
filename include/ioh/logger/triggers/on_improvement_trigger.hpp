#pragma once

#include <set>

#include "ioh/common/log.hpp"
#include "trigger.hpp"
#include "../loginfo.hpp"

namespace ioh
{
    namespace trigger
    {

        /** A trigger that reacts to a strict improvement of the best transformed value.
         *
         * This trigger is used to determine if the logging should occur based on whether
         * the best transformed objective value has strictly improved. It operates on both
         * single-objective and multi-objective problems.
         *
         * @ingroup Triggering
         */
        struct OnImprovement final : logger::Trigger
        {
            /**
             * Operator overload for single-objective problems.
             *
             * This function checks if the best transformed value has strictly improved
             * for single-objective problems and returns true if it has.
             *
             * @param log_info The logging information containing details about the current state.
             * @param meta_data Metadata about the problem (unused in this implementation).
             * @return True if the best transformed value has strictly improved, false otherwise.
             */
            bool operator()(const logger::Info<problem::SingleObjective> &log_info, const problem::MetaData&) override
            {
                IOH_DBG(debug, "trigger OnImprovement called: " << log_info.has_improved);
                return log_info.has_improved;
            }

            /**
             * Operator overload for multi-objective problems.
             *
             * This function checks if the best transformed value has strictly improved
             * for multi-objective problems and returns true if it has.
             *
             * @param log_info The logging information containing details about the current state.
             * @param meta_data Metadata about the problem (unused in this implementation).
             * @return True if the best transformed value has strictly improved, false otherwise.
             */
            bool operator()(const logger::Info<problem::MultiObjective> &log_info, const problem::MetaData&) override
            {
                IOH_DBG(debug, "trigger OnImprovement called: " << log_info.has_improved);
                return log_info.has_improved;
            }
        };

        /** Do log only if the transformed best objective function value found so far has strictly improved.
         *
         * This inline instance of the `OnImprovement` trigger can be used directly in the library
         * to simplify usage without requiring explicit instantiation.
         *
         * @ingroup Triggers
         */
        inline OnImprovement on_improvement; // Uncomment if one wants a library-wide instance.

    } // namespace trigger
} // namespace ioh