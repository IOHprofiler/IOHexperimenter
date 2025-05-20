#pragma once

#include <set>
#include "ioh/common/log.hpp"
#include "trigger.hpp"
#include "../loginfo.hpp"

namespace ioh
{
    namespace trigger
    {
        /** A trigger that fires at a regular interval.
         *
         * This trigger is used to determine whether a logging event should occur
         * based on a fixed interval of evaluations. It starts triggering at a 
         * specified starting point and continues at regular intervals thereafter.
         *
         * @ingroup Triggering
         */ 
        class Each final : public logger::Trigger
        {
        protected:
            //! Period of time (number of evaluations) between triggers.
            const size_t _interval;

            //! Minimum time (number of evaluations) at which to allow triggering.
            const size_t _starting_at;

        public:
            /** Check if the trigger condition is met.
             *
             * This method checks whether the current number of evaluations satisfies
             * the interval condition, starting from the specified starting point.
             *
             * @tparam R The type of the logger information.
             * @param log_info The logger information containing the current state.
             * @return True if the trigger condition is met, false otherwise.
             */
            template<typename R>
            bool each(const logger::Info<R> &log_info)
            {
                IOH_DBG(debug, "trigger Each called");
                if ((log_info.evaluations - _starting_at) % _interval == 0)
                {
                    IOH_DBG(debug, "each triggered " << log_info.evaluations)
                    return true;
                }
                else
                {
                    IOH_DBG(xdebug, "each not triggered " << log_info.evaluations)
                    return false;
                }
            }

            /** Constructor
             *
             * Initializes the trigger with the specified interval and starting point.
             *
             * @param interval Number of evaluations between two triggering events.
             * @param starting_at Minimum time (number of evaluations) at which to start triggering events.
             */
            Each(const size_t interval, const size_t starting_at = 0) 
                : _interval(interval), _starting_at(starting_at)
            {
            }

            //! Accessor for the interval between triggers.
            size_t interval() const { return _interval; }

            //! Accessor for the starting point of the trigger.
            size_t starting_at() const { return _starting_at; }

            /** Main call interface for single-objective problems.
             *
             * This method is called to check whether the trigger condition is met
             * for single-objective problems.
             *
             * @param log_info The logger information containing the current state.
             * @param meta_data Metadata about the problem (not used in this trigger).
             * @return True if the trigger condition is met, false otherwise.
             */
            bool operator()(const logger::Info<problem::SingleObjective> &log_info, const problem::MetaData &) override
            {
                return each(log_info);
            }

            /** Main call interface for multi-objective problems.
             *
             * This method is called to check whether the trigger condition is met
             * for multi-objective problems.
             *
             * @param log_info The logger information containing the current state.
             * @param meta_data Metadata about the problem (not used in this trigger).
             * @return True if the trigger condition is met, false otherwise.
             */
            bool operator()(const logger::Info<problem::MultiObjective> &log_info, const problem::MetaData &) override
            {
                return each(log_info);
            }
        };

        /** Create an `Each` trigger.
         *
         * This function creates an `Each` trigger that logs every given number of
         * function evaluations, starting at a specified point (default is zero).
         *
         * @param interval Number of evaluations between two triggering events.
         * @param starting_at Minimum time (number of evaluations) at which to start triggering events.
         * @return A reference to the created `Each` trigger.
         */
        inline Each &each(const size_t interval, const size_t starting_at = 0)
        {
            const auto t = new Each(interval, starting_at);
            return *t;
        }

    } // namespace trigger
} // namespace ioh