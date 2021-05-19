#pragma once

#include "api.hpp"
#include "../problem/utils.hpp"

namespace ioh {
    namespace trigger {

        /** Combine several triggers in a single one with a logical "and".
         * 
         * By default, the Logger class manage combine several triggers with a logical "or",
         * but you may want to do differently, in which case you can inherit
         * from this class to enjoy the triggers list management,
         * and just do what you want with `operator()`.
         */
        class Combine : public logger::Trigger {
        protected:
            std::vector<std::reference_wrapper<logger::Trigger>> _triggers;
        public:
            Combine( std::initializer_list<std::reference_wrapper<logger::Trigger>> triggers)
            : _triggers(triggers)
            {
                assert(_triggers.size() > 0);
            }

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

            /** Propagate the reset event to all managed triggers. */
            virtual void reset() override
            {
                for(auto& trigger : _triggers) {
                    trigger.get().reset();
                }
            }
        };

        struct Always : public logger::Trigger {
            bool operator()(const log::Info& log_info, const problem::MetaData& pb_info) override
            {
                return true;
            }
        } always;

        class OnImprovement : public logger::Trigger {
        protected:
            double _best;
            const common::OptimizationType _type;
        public:
            OnImprovement(const problem::MetaData& pb_info)
            : _type(pb_info.optimization_type)
            {
                reset();
            }
            
            bool operator()(const log::Info& log_info, const problem::MetaData& pb_info) override
            {
                // We do not use log::Info::transformed_y_best below,
                // because all fields of log::Info are updated before the trigger see them.
                // That would force to test for equality to trigger on improvement,
                // and we only want to trigger on strict inequality.
                if(common::compare_objectives(log_info.transformed_y, _best, _type)) {
                    _best = log_info.transformed_y;
                    return true;
                }
                return false;
            }

            void reset() override
            {
                if(_type == common::OptimizationType::Minimization) {
                    _best =  std::numeric_limits<double>::infinity();
                } else {
                    _best = -std::numeric_limits<double>::infinity();
                }
            }
        };

        // TODO HERE: AtInterval AtTimePoints PerTimeRange
    } // trigger
} // ioh
