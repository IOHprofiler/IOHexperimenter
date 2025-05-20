#pragma once

#include <set>

#include "ioh/common/log.hpp"
#include "trigger.hpp"
#include "../loginfo.hpp"

namespace ioh
{
    namespace trigger
    {

        /** 
         * Interface to combine several triggers into a single one.
         *
         * By default, the Logger class combines several triggers with a logical "or" (see trigger::Any),
         * but you may want to do so differently. In such cases, you can inherit
         * from this class to manage the triggers list and implement custom behavior
         * in the `operator()` method.
         *
         * @ingroup Triggering
         */
        class Set : public logger::Trigger
        {
        protected:
            //! The list of managed triggers.
            logger::Triggers triggers_;

        public:
            /** 
             * Default constructor.
             *
             * @warning This constructor is primarily intended for internal use 
             * (e.g., for the "_any" default member of Logger).
             */
            Set() {}

            /** 
             * Constructor with triggers.
             *
             * @param triggers The list of triggers to manage.
             */
            Set(logger::Triggers triggers) : triggers_(triggers) {}

            /** 
             * Propagate the reset event to all managed triggers.
             *
             * This method ensures that all triggers in the set are reset.
             */
            virtual void reset() override
            {
                IOH_DBG(debug, "reset triggers in Set");
                for (auto &trigger : triggers_)
                {
                    trigger.get().reset();
                }
            }

            /** 
             * Add a trigger to the set.
             *
             * @param trigger The trigger to add.
             */
            void push_back(logger::Trigger &trigger) { triggers_.push_back(std::ref(trigger)); }

            /** 
             * Add a trigger to the set.
             *
             * @param trigger The trigger to add.
             */
            void insert(logger::Trigger &trigger) { triggers_.push_back(std::ref(trigger)); }

            /** 
             * Get the number of managed triggers.
             *
             * @return The number of triggers in the set.
             */
            size_t size() { return triggers_.size(); }

            /** 
             * Get a copy of the managed triggers.
             *
             * @return A copy of the triggers.
             */
            [[nodiscard]] logger::Triggers triggers() const { return triggers_; }
        };

    } // namespace trigger
} // namespace ioh