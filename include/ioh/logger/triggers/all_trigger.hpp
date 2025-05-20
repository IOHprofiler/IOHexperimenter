#pragma once

#include <set>

#include "ioh/common/log.hpp"
#include "trigger.hpp"
#include "set_trigger.hpp"
#include "../loginfo.hpp"

namespace ioh
{
    namespace trigger
    {

        /** Combine several triggers in a single one with a logical "and".
         *
         * The `All` trigger ensures that all the managed triggers must be fired
         * for it to be considered triggered. This is useful for combining multiple
         * conditions into a single logical "and" operation.
         *
         * @ingroup Triggering
         */
        struct All final : public Set
        {
            /** Check if all triggers are fired.
             *
             * This method iterates through all the managed triggers and ensures
             * that each one is fired. If any trigger is not fired, it returns false.
             *
             * @tparam R The type of the logger information (e.g., SingleObjective or MultiObjective).
             * @param log_info The logger information containing runtime data.
             * @param pb_info The problem metadata.
             * @return True if all triggers are fired, false otherwise.
             */
            template<typename R>
            bool all(const logger::Info<R> &log_info, const problem::MetaData &pb_info)
            {
                IOH_DBG(debug, "trigger All called");
                assert(this->triggers_.size() > 0);
                for (auto &trigger : this->triggers_)
                {
                    if (not trigger(log_info, pb_info))
                    {
                        IOH_DBG(xdebug, "not all triggered");
                        return false;
                    }
                }
                IOH_DBG(debug, "all triggered");
                return true;
            }

            /** Default constructor.
             *
             * Creates an empty `All` trigger. This constructor is not recommended
             * for use as it results in an empty set of triggers.
             *
             * @warning You should probably not use this one.
             */
            All() : Set() {}

            /** Constructor with triggers.
             *
             * Initializes the `All` trigger with a set of managed triggers.
             *
             * @param triggers The managed triggers to be combined.
             */
            All(logger::Triggers triggers) : Set(triggers) {}
        

            /** Check if all triggers are fired (SingleObjective specialization).
             *
             * This operator is called to evaluate the `All` trigger for
             * SingleObjective problems.
             *
             * @param log_info The logger information containing runtime data.
             * @param pb_info The problem metadata.
             * @return True if all triggers are fired, false otherwise.
             */
            virtual bool operator()(const logger::Info<problem::SingleObjective> &log_info, const problem::MetaData &pb_info) override
            {
                return all(log_info, pb_info);
            }

            /** Check if all triggers are fired (MultiObjective specialization).
             *
             * This operator is called to evaluate the `All` trigger for
             * MultiObjective problems.
             *
             * @param log_info The logger information containing runtime data.
             * @param pb_info The problem metadata.
             * @return True if all triggers are fired, false otherwise.
             */
            virtual bool operator()(const logger::Info<problem::MultiObjective> &log_info, const problem::MetaData &pb_info) override
            {
                return all(log_info, pb_info);
            }
        };

        /** Create an `All` trigger.
         *
         * This function creates an `All` trigger that combines multiple triggers
         * into a single logical "and" operation.
         *
         * @param triggers The triggers to be combined.
         * @return A reference to the created `All` trigger.
         *
         * @ingroup Triggers
         */
        inline All &all(logger::Triggers triggers)
        {
            const auto t = new All(triggers);
            return *t;
        }

    } // namespace trigger
} // namespace ioh