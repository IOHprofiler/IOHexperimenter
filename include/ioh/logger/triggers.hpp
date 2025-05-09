#pragma once

#include <set>

#include "ioh/common/log.hpp"
#include "loginfo.hpp"

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
        template<typename R>
        struct Trigger
        {

            /** @returns true if a log event is to be triggered given the passed state. */
            virtual bool operator()(const logger::Info<R> &log_info, const problem::MetaData &pb_info) = 0;
            
            /** Reset any internal state.
             *
             * @note This is called when the logger is attached to a new problem/run/etc.
             *
             * Useful if, for instance, the trigger maintain its own "best value so far" (@see logger::OnImprovement).
             */
            virtual void reset() {}

            virtual ~Trigger() = default;
        };

        /**
         * \brief convenience typedef for a vector of triggers
         */
        template <typename R>
        using Triggers = std::vector<std::reference_wrapper<Trigger<R>>>;

    } // namespace logger

    /** Everything related to triggering a logger event. */
    namespace trigger
    {

        /** Interface to combine several triggers in a single one.
         *
         * By default, the Logger class manage combine several triggers with a logical "or" (see trigger::Any),
         * but you may want to do differently, in which case you can inherit
         * from this class to enjoy the triggers list management,
         * and just do what you want with `operator()`.
         *
         * @ingroup Triggering
         */
        template<typename R>
        class Set : public logger::Trigger<R>
        {
        protected:
            //! Managed triggers.
            logger::Triggers<R> triggers_;

        public:
            /** Empty constructor
             *
             * @warning You should probably not use this one.
             */
            // Empty constructor are needed (at least for the "_any" default member of Logger).
            Set() {}

            /** Constructor
             *
             * @param triggers the managed triggers.
             */
            Set(logger::Triggers<R> triggers) : triggers_(triggers) {}

            // virtual bool operator()(const logger::Info& log_info, const problem::MetaData& pb_info) = 0;

            /** Propagate the reset event to all managed triggers. */
            virtual void reset() override
            {
                IOH_DBG(debug, "reset triggers in Set");
                for (auto &trigger : triggers_)
                {
                    trigger.get().reset();
                }
            }

            //! Add a trigger to the set.
            void push_back(logger::Trigger<R> &trigger) { triggers_.push_back(std::ref(trigger)); }

            //! Add a trigger to the set.
            void insert(logger::Trigger<R> &trigger) { triggers_.push_back(std::ref(trigger)); }

            //! Get the number of managed triggers.
            size_t size() { return triggers_.size(); }

            //! Get a copy of the triggers
            [[nodiscard]] logger::Triggers<R> triggers() const { return triggers_; }
        };

        /** Combine several triggers in a single one with a logical "or".
         *
         * @note This is use as the default combination when you pass several triggers to a logger.
         *
         * @ingroup Triggering
         */
        template<typename R>
        struct Any final : public Set<R>
        {

            /** Empty constructor
             *
             * @warning You should probably not use this one.
             */
            Any() : Set<R>() {}

            /** Constructor
             *
             * @param triggers the managed triggers.
             */
            Any(logger::Triggers<R> triggers) : Set<R>(triggers) {}

            /** Triggered if ANY the managed triggers are triggered. */
            virtual bool operator()(const logger::Info<R> &log_info, const problem::MetaData &pb_info) override
            {
                IOH_DBG(debug, "trigger Any called");
                assert(!this->triggers_.empty());
                for (auto &trigger : this->triggers_)
                {
                    if (trigger(log_info, pb_info))
                    {
                        IOH_DBG(debug, "any triggered")
                        return true;
                    }
                }
                IOH_DBG(xdebug, "not any triggered")
                return false;
            }
        };
        /** Do log if ANY of the given triggers is fired.
         *
         * @ingroup Triggers
         */
        template<typename R>
        inline Any<R> &any(logger::Triggers<R> triggers)
        {
            const auto t = new Any<R>(triggers);
            return *t;
        }

        /** Combine several triggers in a single one with a logical "and".
         *
         * @ingroup Triggering
         */
        template<typename R>
        struct All final : public Set<R>
        {

            /** Empty constructor.
             *
             * @warning You should probably not use this one directly.
             */
            All() : Set<R>() {}

            /** Constructor
             *
             * @param triggers the set of Trigger to manage.
             */
            All(logger::Triggers<R> triggers) : Set<R>(triggers) {}

            /** Triggered if ALL the managed triggers are triggered. */
            virtual bool operator()(const logger::Info<R> &log_info, const problem::MetaData &pb_info) override
            {
                IOH_DBG(debug, "trigger All called");
                assert(this->triggers_.size() > 0);
                for (auto &trigger : this->triggers_)
                {
                    if (not trigger(log_info, pb_info))
                    {
                        IOH_DBG(xdebug, "not all triggered")
                        return false;
                    }
                }
                IOH_DBG(debug, "all triggered")
                return true;
            }
        };
        /** Do log if ALL the given triggers are fired.
         *
         * @ingroup Triggers
         */
        template<typename R>
        inline All<R> &all(logger::Triggers<R> triggers)
        {
            const auto t = new All(triggers);
            return *t;
        }

        /** A trigger that always returns `true`.
         *
         * @ingroup Triggering
         */
        template<typename R>
        struct Always final : public logger::Trigger<R>
        {
            bool operator()(const logger::Info<R> &, const problem::MetaData &) override
            {
                IOH_DBG(debug, "always triggered")
                return true;
            }
        };
        /** Do log at every call of the objective function.
         *
         * @ingroup Triggers
         */
        template<typename R>
        inline Always<R> always; // Uncomment if one want a library.

        /** A trigger that react to a strict improvement of the best transformed value.
         *
         * @ingroup Triggering
         */
        template<typename R>
        struct OnImprovement final : logger::Trigger<R>
        {
            bool operator()(const logger::Info<R> &log_info, const problem::MetaData&) override
            {
                IOH_DBG(debug, "trigger OnImprovement called: " << log_info.has_improved);
                return log_info.has_improved;
            }
        };
        /** Do log only if the transformed best objective function value found so far has strictly improved.
         *
         * @ingroup Triggers
         */
        template<typename R>
        inline OnImprovement<R> on_improvement; // Uncomment if one want a library.

        template<typename R>
        struct OnDeltaImprovement final : logger::Trigger<R> {
            double delta;
            double best_so_far;

            explicit OnDeltaImprovement(const double delta = 1e-10): delta(delta) {
                reset();
            }

            OnDeltaImprovement(const double delta, const double best_so_far): delta(delta), best_so_far(best_so_far) {
            }
            
            bool operator()(const logger::Info<R> &log_info, const problem::MetaData &pb_info) override {
                // if (std::isnan(best_so_far)){
                //     best_so_far = log_info.y;
                //     return true;
                // }

                // if (pb_info.optimization_type(log_info.y, best_so_far) && std::abs(best_so_far - log_info.y) > delta) {
                //     best_so_far = log_info.y;
                //     return true;
                // }                
                // return false;
                return true;
            }

            void reset() override {
                best_so_far = std::numeric_limits<double>::signaling_NaN();
            }
        };

        /** Do log only if the transformed best objective function value found so far has strictly improved by delta
         *
         * @ingroup Triggers
         */
        template<typename R>
        inline OnDeltaImprovement<R> on_delta_improvement;

        //! Trigger when there is constraint violation
        template<typename R>
        struct OnViolation final : logger::Trigger<R> {
            //! Track the number of violations
            int violations{};

            //! Call interface
            bool operator()(const logger::Info<R> &log_info, const problem::MetaData&) override
            {
                const bool violation = log_info.violations[0] != 0.;
                IOH_DBG(debug, "trigger OnViolation called: " << violation);
                violations += violation;
                return violation;
            }

            //! Reset the violations counter
            void reset() override {
                violations = 0;
            }
        };

        //! Log when there are violations
        template<typename R>
        inline OnViolation<R> on_violation;

        /** A trigger that fire at a regular interval.
         *
         * @ingroup Triggering
         */
        template<typename R> 
        class Each final : public logger::Trigger<R>
        {
        protected:
            //! Period of time between triggers.
            const size_t _interval;
            //! Minimum time at which to allow triggering.
            const size_t _starting_at;

        public:
            /** Constructor
             *
             * @param interval number of evaluations between two triggering events.
             * @param starting_at minimum time at which to start triggering events.
             */
            Each(const size_t interval, const size_t starting_at = 0) : _interval(interval), _starting_at(starting_at)
            {
            }
            //! Accessor for _interval
            size_t interval() const { return _interval; }

            //! Accessor for _starting_at
            size_t starting_at() const { return _starting_at; }

            //! Main call interface.
            bool operator()(const logger::Info<R> &log_info, const problem::MetaData &) override
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
        };
        /** Do log every given number of function evaluations.
         *
         * Starts counting at zero by default.
         *
         * @ingroup Triggers
         */
        template<typename R>
        inline Each<R> &each(const size_t interval, const size_t starting_at = 0)
        {
            const auto t = new Each(interval, starting_at);
            return *t;
        }

        /** A trigger that fire only at specific number of evaluations.
         *
         * @ingroup Triggering
         */
        template<typename R>
        class At final : public logger::Trigger<R>
        {
        protected:
            //! Set of times at which to trigger events.
            const std::set<size_t> _time_points;

            //! Check if time is in the managed set.
            bool matches(const size_t evals) { return _time_points.find(evals) != std::end(_time_points); }

        public:
            /** Constructor.
             *
             * @param time_points the set of evaluations for which to trigger an event.
             */
            At(const std::set<size_t> time_points) : _time_points(time_points) {}

            //! Return the time points when to log
            std::set<size_t> time_points() const { return _time_points; }

            //! Main call interface.
            bool operator()(const logger::Info<R> &log_info, const problem::MetaData &) override
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
        };
        /** Do log at the given number of evaluations.
         *
         * @ingroup Triggers
         */
        template<typename R>
        inline At<R> &at(const std::set<size_t> time_points)
        {
            const auto t = new At<R>(time_points);
            return *t;
        }

        /** A trigger that fire only when the number of evaluations is within certain range(s).
         *
         * @note Ranges are closed intervals: with `[start,end]`, the trigger will fire for start and end.
         *       Ranges may overlap with each other.
         *
         * @ingroup Triggering
         */
        template<typename R>   
        class During final : public logger::Trigger<R>
        {
        protected:
            //! Time ranges during which events are triggered.
            const std::set<std::pair<size_t, size_t>> _time_ranges;

            //! Check if a time is in one of the ranges.
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
             * @param time_ranges the set of [min_evals,max_evals] during which events will be triggered.
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
            //! Accessor for _time_ranges
            std::set<std::pair<size_t, size_t>> time_ranges() const { return _time_ranges; }

            //! Main call interface.
            bool operator()(const logger::Info<R> &log_info, const problem::MetaData &) override
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
        };
        /** Do log if the number of evaluations falls in at least one of the given ranges.
         *
         * @note Ranges are closed intervals `[start,end]`, the trigger will fire for start and end.
         *       Ranges may overlap with each other.
         *
         * @ingroup Triggers
         */
        template<typename R>
        inline During<R> &during(const std::set<std::pair<size_t, size_t>> time_ranges)
        {
            const auto t = new During<R>(time_ranges);
            return *t;
        }

    } // namespace trigger
} // namespace ioh
