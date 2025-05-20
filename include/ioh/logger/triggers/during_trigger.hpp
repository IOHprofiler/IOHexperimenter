#pragma once

#include <set>
#include "ioh/common/log.hpp"
#include "trigger.hpp"
#include "../loginfo.hpp"

namespace ioh
{
    namespace trigger
    {
        /** A trigger that fires only when the number of evaluations is within certain range(s).
         *
         * @note Ranges are closed intervals: with `[start,end]`, the trigger will fire for both `start` and `end`.
         *       Ranges may overlap with each other.
         *
         * @ingroup Triggering
         */
        class During final : public logger::Trigger
        {
        private:
            /** Check if the current number of evaluations falls within any of the specified ranges.
             *
             * @tparam R The type of the logger information.
             * @param log_info The logger information containing the current number of evaluations.
             * @return True if the current number of evaluations is within any range, false otherwise.
             */
            template<typename R>
            bool during(const logger::Info<R> &log_info)
            {
                IOH_DBG(debug, "trigger During called");
                if (matches(log_info.evaluations))
                {
                    IOH_DBG(debug, "triggered during " << log_info.evaluations)
                    return true;
                }
                else
                {
                    IOH_DBG(xdebug, "not triggered during " << log_info.evaluations)
                    return false;
                }
            }

        protected:
            //! Time ranges during which events are triggered.
            const std::set<std::pair<size_t, size_t>> _time_ranges;

            /** Check if a given number of evaluations falls within one of the time ranges.
             *
             * @param evals The current number of evaluations.
             * @return True if the number of evaluations is within any range, false otherwise.
             */
            bool matches(const size_t evals)
            {
                // TODO Use binary search to speed-up ranges tests.
                for (const auto &r : _time_ranges)
                {
                    assert(r.first <= r.second);
                    if (r.first <= evals and evals <= r.second)
                    {
                        return true;
                    }
                }
                return false;
            }

        public:
            /** Constructor.
             *
             * @param time_ranges The set of `[min_evals, max_evals]` ranges during which events will be triggered.
             */
            During(std::set<std::pair<size_t, size_t>> time_ranges) : _time_ranges(time_ranges)
            {
#ifndef NDEBUG
                assert(not _time_ranges.empty());
                for (const auto &r : _time_ranges)
                {
                    assert(r.first <= r.second); // less or equal, because it can be a single time point.
                }
#endif
            }

            /** Accessor for the time ranges.
             *
             * @return The set of time ranges during which events are triggered.
             */
            std::set<std::pair<size_t, size_t>> time_ranges() const { return _time_ranges; }

            /** Main call interface for single-objective problems.
             *
             * @param log_info The logger information containing the current number of evaluations.
             * @param meta_data Metadata about the problem (unused in this implementation).
             * @return True if the trigger condition is met, false otherwise.
             */
            bool operator()(const logger::Info<problem::SingleObjective> &log_info, const problem::MetaData &) override
            {
                return during(log_info);
            }

            /** Main call interface for multi-objective problems.
             *
             * @param log_info The logger information containing the current number of evaluations.
             * @param meta_data Metadata about the problem (unused in this implementation).
             * @return True if the trigger condition is met, false otherwise.
             */
            bool operator()(const logger::Info<problem::MultiObjective> &log_info, const problem::MetaData &) override
            {
                return during(log_info);
            }
        };

        /** Create a `During` trigger with the specified time ranges.
         *
         * @param time_ranges The set of `[min_evals, max_evals]` ranges during which events will be triggered.
         * @return A reference to the newly created `During` trigger.
         */
        inline During &during(const std::set<std::pair<size_t, size_t>> time_ranges)
        {
            const auto t = new During(time_ranges);
            return *t;
        }

    } // namespace trigger
} // namespace ioh