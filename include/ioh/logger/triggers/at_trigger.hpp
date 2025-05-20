#pragma once

#include <set>
#include "ioh/common/log.hpp"
#include "trigger.hpp"
#include "../loginfo.hpp"

namespace ioh
{
    namespace trigger
    {
        /** A trigger that fires only at specific numbers of evaluations.
         *
         * This class allows triggering events at predefined evaluation points.
         * It is useful for logging or other operations that need to occur
         * at specific intervals during the execution of an algorithm.
         *
         * @ingroup Triggering
         */
        class At final : public logger::Trigger
        {
            /**
             * Check if the current evaluation matches one of the predefined time points.
             *
             * @tparam R The type of the result (e.g., SingleObjective or MultiObjective).
             * @param log_info The logging information containing the current evaluation count.
             * @return True if the current evaluation matches a predefined time point, false otherwise.
             */
            template<typename R>
            bool at(const logger::Info<R> &log_info)
            {
                IOH_DBG(debug, "trigger At called");
                if (matches(log_info.evaluations))
                {
                    IOH_DBG(debug, "triggered at " << log_info.evaluations)
                    return true;
                }
                else
                {
                    IOH_DBG(xdebug, "not triggered at " << log_info.evaluations)
                    return false;
                }
            }

        protected:
            //! Set of times at which to trigger events.
            const std::set<size_t> _time_points;

            /**
             * Check if a given evaluation count is in the managed set of time points.
             *
             * @param evals The current evaluation count.
             * @return True if the evaluation count is in the set, false otherwise.
             */
            bool matches(const size_t evals) { return _time_points.find(evals) != std::end(_time_points); }

        public:
            /**
             * Constructor.
             *
             * Initializes the trigger with a set of evaluation points at which to trigger events.
             *
             * @param time_points The set of evaluations for which to trigger an event.
             */
            At(const std::set<size_t> time_points) : _time_points(time_points) {}

            /**
             * Get the time points at which the trigger is set to log.
             *
             * @return A set of evaluation points.
             */
            std::set<size_t> time_points() const { return _time_points; }

            /**
             * Main call interface for SingleObjective problems.
             *
             * @param log_info The logging information containing the current evaluation count.
             * @param meta_data Metadata about the problem (not used in this implementation).
             * @return True if the trigger condition is met, false otherwise.
             */
            bool operator()(const logger::Info<problem::SingleObjective> &log_info, const problem::MetaData &) override
            {
                return at(log_info);
            }

            /**
             * Main call interface for MultiObjective problems.
             *
             * @param log_info The logging information containing the current evaluation count.
             * @param meta_data Metadata about the problem (not used in this implementation).
             * @return True if the trigger condition is met, false otherwise.
             */
            bool operator()(const logger::Info<problem::MultiObjective> &log_info, const problem::MetaData &) override
            {
                return at(log_info);
            }
        };

        /**
         * Create an At trigger for the given set of evaluation points.
         *
         * This function creates a new At trigger and returns a reference to it.
         * The caller is responsible for managing the lifetime of the returned object.
         *
         * @param time_points The set of evaluations for which to trigger an event.
         * @return A reference to the created At trigger.
         */
        inline At &at(const std::set<size_t> time_points)
        {
            const auto t = new At(time_points);
            return *t;
        }

    } // namespace trigger
} // namespace ioh