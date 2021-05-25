#pragma once

// #include "api.hpp"
#include "../problem/utils.hpp"

namespace ioh {
    namespace logger {

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
         * @ingroup Loggers
         */
        
        /** Interface for classes triggering a log event.
         *
         * @ingroup Triggering
         */
        struct Trigger {

            /** @returns true if a log event is to be triggered given the passed state. */
            virtual bool operator()(const log::Info& log_info, const problem::MetaData& pb_info) = 0;

            /** Reset any internal state.
             * 
             * @note This is called when the logger is attached to a new problem/run/etc.
             * 
             * Useful if, for instance, the trigger maintain its own "best value so far" (@see logger::OnImprovement).
             */
            virtual void reset() { }
        };
        
    } // logger

    /** Everything related to triggering a logger event. */
    namespace trigger {

        /** Interface to combine several triggers in a single one.
         * 
         * By default, the Logger class manage combine several triggers with a logical "or" (see trigger::Any),
         * but you may want to do differently, in which case you can inherit
         * from this class to enjoy the triggers list management,
         * and just do what you want with `operator()`.
         * 
         * @ingroup Triggering
         */
        class Set : public logger::Trigger {
            protected:
                std::vector<std::reference_wrapper<logger::Trigger>> _triggers;

            public:
                // Empty constructor are needed (at least for the "_any" default member of Logger).
                Set(){}

                Set( std::vector<std::reference_wrapper<logger::Trigger>> triggers)
                : _triggers(triggers)
                { }

                // virtual bool operator()(const log::Info& log_info, const problem::MetaData& pb_info) = 0;
            
                /** Propagate the reset event to all managed triggers. */
                virtual void reset() override
                {
                    for(auto& trigger : _triggers) {
                        trigger.get().reset();
                    }
                }

                void push_back(logger::Trigger& trigger) { _triggers.push_back(trigger); }

                size_t size() { return _triggers.size(); }

        };

        /** Combine several triggers in a single one with a logical "or".
         * 
         * @note This is use as the default combination when you pass several triggers to a logger.
         * 
         * @ingroup Triggering
         */
        struct Any: public Set {

            Any():Set(){}

            Any( std::vector<std::reference_wrapper<logger::Trigger>> triggers)
            : Set(triggers)
            { }

            /** Triggered if ANY the managed triggers are triggered. */
            virtual bool operator()(const log::Info& log_info, const problem::MetaData& pb_info) override
            {
                for(auto& trigger : _triggers) {
                    if(trigger(log_info, pb_info)) {
                        return true;
                    }
                }
                return false;
            }
        };
        /** Do log if ANY of the given triggers is fired.
         * 
         * @ingroup Triggers
         */
        Any& any( std::vector<std::reference_wrapper<logger::Trigger>> triggers )
        {
            auto t = std::make_shared<Any>(triggers);
            return *t;
        }
        
        /** Combine several triggers in a single one with a logical "and".
         * 
         * @ingroup Triggering
         */
        struct All: public Set {

            All():Set(){}

            All( std::vector<std::reference_wrapper<logger::Trigger>> triggers)
            : Set(triggers)
            { }

            /** Triggered if ALL the managed triggers are triggered. */
            virtual bool operator()(const log::Info& log_info, const problem::MetaData& pb_info) override
            {
                for(auto& trigger : _triggers) {
                    if(not trigger(log_info, pb_info)) {
                        return false;
                    }
                }
                return true;
            }

        };
        /** Do log if ALL the given triggers are fired.
         *
         * @ingroup Triggers
         */
        All& all( std::vector<std::reference_wrapper<logger::Trigger>> triggers )
        {
            auto t = std::make_shared<All>(triggers);
            return *t;
        }

        /** A trigger that always returns `true`.
         * 
         * @ingroup Triggering
         */
        struct Always : public logger::Trigger {
            bool operator()(const log::Info& log_info, const problem::MetaData& pb_info) override
            {
                return true;
            }
        };
        /** Do log at every call of the objective function.
         *
         * @ingroup Triggers
         */
        Always always;

        /** A trigger that react to a strict improvement of the best transformed value.
         * 
         * @ingroup Triggering
         */
        class OnImprovement : public logger::Trigger {
        protected:
            double _best;
            common::OptimizationType _type;

            // State management flag, avoids being dependent on a problem at construction, 
            // to the expense of a test at each call.
            bool _has_type;

        public:
            OnImprovement()
            : _has_type(false)
            {
                reset();
            }
            
            bool operator()(const log::Info& log_info, const problem::MetaData& pb_info) override
            {
                if(not _has_type) {
                    _type = pb_info.optimization_type;
                    if(_type == common::OptimizationType::Minimization) {
                        _best =  std::numeric_limits<double>::infinity();
                    } else {
                        _best = -std::numeric_limits<double>::infinity();
                    }
                    _has_type = true;
                }
                // We do not use log::Info::transformed_y_best below,
                // because all fields of log::Info are updated before the trigger see them.
                // That would force to test for equality to trigger on improvement,
                // and we only want to trigger on strict inequality.
                assert(_has_type);
                if(common::compare_objectives(log_info.transformed_y, _best, _type)) {
                    _best = log_info.transformed_y;
                    return true;
                }
                return false;
            }

            void reset() override
            {
                _has_type = false;
            }
        };
        /** Do log only if the transformed best objective function value found so far has strictly improved.
         * 
         * @ingroup Triggers
         */
        OnImprovement on_improvement;

        /** A trigger that fire at a regular interval.
         * 
         * @ingroup Triggering
         */
        class AtInterval : public logger::Trigger {
        protected:
            const size_t _interval;
            const size_t _starting_at;

        public:
            AtInterval(const size_t interval, const size_t starting_at = 0)
            : _interval(interval)
            , _starting_at(starting_at)
            { }

            bool operator()(const log::Info& log_info, const problem::MetaData& pb_info) override
            {
                if((log_info.evaluations-_starting_at) % _interval == 0) {
                    return true;
                } else {
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
        AtInterval& each(const size_t interval, const size_t starting_at = 0)
        {
           auto t = std::make_shared<AtInterval>(interval, starting_at);
           return *t;
        }

        /** A trigger that fire only at specific number of evaluations.
         *
         * @ingroup Triggering
         */
        class AtTimePoints : public logger::Trigger {
        protected:
            const std::set<size_t> _time_points;

            bool matches(const size_t evals)
            {
                return std::find( std::begin(_time_points), std::end(_time_points), evals)
                       != std::end(_time_points);
            }

        public:
            AtTimePoints(const std::set<size_t> time_points)
            : _time_points(time_points)
            {
                assert(not time_points.empty());
            }

            bool operator()(const log::Info& log_info, const problem::MetaData& pb_info) override
            {
                if(matches(log_info.evaluations)) {
                    return true;
                } else {
                    return false;
                }
            }
        };
        /** Do log at the given number of evaluations.
         * 
         * @ingroup Triggers
         */
        AtTimePoints& at(const std::set<size_t> time_points)
        {
            auto t = std::make_shared<AtTimePoints>(time_points);
            return *t;
        }

        /** A trigger that fire only when the number of evaluations is within certain range(s).
         *
         * @note Ranges are closed intervals: with `[start,end]`, the trigger will fire for start and end.
         *       Ranges may overlap with each other.
         * 
         * @ingroup Triggering
         */
        class PerTimeRange : logger::Trigger {
        protected:
            const std::set<std::pair<size_t,size_t>> _time_ranges;

            bool matches(const size_t evals)
            {
                for(const auto& r : _time_ranges) {
                    assert(r.first <= r.second);
                    if(r.first <= evals and evals <= r.second) {
                        return true;
                    }
                }
                return false;
            }

        public:
            PerTimeRange( std::set<std::pair<size_t,size_t>> time_ranges )
            : _time_ranges(time_ranges)
            {
                assert(not _time_ranges.empty());
#ifndef NDEBUG
                for(const auto& r : _time_ranges) {
                    assert(r.first <= r.second); // less or equal, because it can be a single time point.
                }
#endif
            }

            bool operator()(const log::Info& log_info, const problem::MetaData& pb_info) override
            {
               if(matches(log_info.evaluations)) {
                   return true;
               } else {
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
        PerTimeRange& during(const std::set<std::pair<size_t,size_t>> time_ranges)
        {
            auto t = std::make_shared<PerTimeRange>(time_ranges);
            return *t;
        }

    } // trigger
} // ioh
