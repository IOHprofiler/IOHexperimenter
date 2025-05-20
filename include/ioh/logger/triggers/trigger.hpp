#pragma once

#include <set>

#include "ioh/common/log.hpp"
#include "ioh/logger/loginfo.hpp"

namespace ioh
{
    namespace logger
    {

        /** @defgroup Triggering Triggering
         * Everything related to the triggering API.
         *
         * @ingroup API
         */

        /** @defgroup Triggers Triggers
         * Events determining when to do a log event.
         *
         * Examples:
         * @code
            // Fire log events only when the best transformed objective function value found so far has improved.
            ioh::logger::Store store_all_bests({ioh::trigger::on_improvement},{ioh::watch::transformed_y_best});

            // Do log regularly along the run, or if there is an improvement.
            ioh::logger::Store store_regularly({ // Several triggers defaults to trigger::any. Thus, fires if:
                    ioh::trigger::each(10),      // each 10 calls, OR...
                    ioh::trigger::on_improvement // on improvement.
                }, {ioh::watch::transformed_y_best});

            // Do log at the 10 first and 10 last calls of a 100-evaluations run.
            ioh::logger::Store store_some({ioh::trigger::during({{0,10},{90,100}})}, {ioh::watch::transformed_y_best});

            // Do log if there is improvement at the beginning or the end, but not in between.
            ioh::logger::Store complex_store({ioh::trigger::all({ // Fires only if all sub-triggers do fire, that is if:
                    ioh::trigger::on_improvement,                 // there is improvement, AND...
                    ioh::trigger::during({{0,10},{90,100}})       // at the beginning or end.
                }, {ioh::watch::transformed_y_best});
         * @endcode
         *
         * @warning Convenience instances are global variables provided for their ease of use.
         *          However, some of them manage a state (like trigger::on_improvement).
         *          Thus, think twice before using them in several loggers!
         *          In some rare cases, you may want to have two copies, to avoid side effects.
         *
         * Some triggers need to be instantiated with parameters before being passed to a logger,
         * you can use the related free functions (lower-case name) to do so on the heap.
         *
         * @warning Those free functions do allocate on the heap, so you're responsible of freeing memory after them if
         necessary.
         *
         * For example:
             @code
                auto t& = trigger:at({1,10,100});
                // [Use t...]
                delete &t;
             @endcode
         *
         * @ingroup Loggers
         */

        /** Interface for classes triggering a log event.
         *
         * @ingroup Triggering
         */
        struct Trigger
        {

            /** @brief Determines if a log event should be triggered based on the current state.
             *
             * @param log_info The logging information for a single-objective problem.
             * @param pb_info The metadata of the problem.
             * @returns true if a log event is to be triggered, false otherwise.
             */
            virtual bool operator()(const logger::Info<problem::SingleObjective> &log_info, const problem::MetaData &pb_info) = 0;
            
            /** @brief Determines if a log event should be triggered based on the current state.
             *
             * @param log_info The logging information for a multi-objective problem.
             * @param pb_info The metadata of the problem.
             * @returns true if a log event is to be triggered, false otherwise.
             */
            virtual bool operator()(const logger::Info<problem::MultiObjective> &log_info, const problem::MetaData &pb_info) = 0;
            
            
            /** @brief Resets any internal state of the trigger.
             *
             * @note This is called when the logger is attached to a new problem, run, or other context.
             *
             * Useful if, for instance, the trigger maintains its own "best value so far" (@see logger::OnImprovement).
             */
            virtual void reset() {}

            /** @brief Virtual destructor for the Trigger interface. */
            virtual ~Trigger() = default;
        };

        /**
         * @brief Convenience typedef for a vector of triggers.
         *
         * This allows grouping multiple triggers together for use in logging operations.
         */
        using Triggers = std::vector<std::reference_wrapper<Trigger>>;

    } // namespace logger
} // namespace ioh