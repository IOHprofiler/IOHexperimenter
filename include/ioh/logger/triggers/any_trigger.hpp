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

        /** Combine several triggers in a single one with a logical "or".
         *
         * This class allows combining multiple triggers such that if any one of them is triggered,
         * the combined trigger will also be triggered. It is useful for scenarios where multiple
         * conditions need to be monitored, and an action should be taken if any of them are met.
         *
         * @note This is used as the default combination when you pass several triggers to a logger.
         *
         * @ingroup Triggering
         */
        struct Any final : public Set
        {
            /** Check if any of the managed triggers is triggered.
             *
             * This method iterates over all the triggers managed by this instance and checks if
             * any of them is triggered based on the provided log information and problem metadata.
             *
             * @tparam R The type of the logger information (e.g., SingleObjective or MultiObjective).
             * @param log_info The logger information containing runtime data.
             * @param pb_info The metadata of the problem being solved.
             * @return True if any of the triggers is triggered, false otherwise.
             */
            template<typename R>
            bool any(const logger::Info<R> &log_info, const problem::MetaData &pb_info)
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

            /** Default constructor.
             *
             * Creates an empty `Any` trigger. This constructor is primarily for internal use,
             * and it is not recommended to use it directly.
             *
             * @warning You should probably not use this one.
             */
            Any() : Set() {}

            /** Constructor with triggers.
             *
             * Initializes the `Any` trigger with a set of managed triggers.
             *
             * @param triggers The triggers to be managed by this instance.
             */
            Any(logger::Triggers triggers) : Set(triggers) {}

            /** Check if any of the managed triggers is triggered for SingleObjective problems.
             *
             * This operator overload is called when the logger information corresponds to
             * SingleObjective problems.
             *
             * @param log_info The logger information containing runtime data.
             * @param pb_info The metadata of the problem being solved.
             * @return True if any of the triggers is triggered, false otherwise.
             */
            virtual bool operator()(const logger::Info<problem::SingleObjective> &log_info, const problem::MetaData &pb_info) override
            {
                return any(log_info, pb_info);
            }

            /** Check if any of the managed triggers is triggered for MultiObjective problems.
             *
             * This operator overload is called when the logger information corresponds to
             * MultiObjective problems.
             *
             * @param log_info The logger information containing runtime data.
             * @param pb_info The metadata of the problem being solved.
             * @return True if any of the triggers is triggered, false otherwise.
             */
            virtual bool operator()(const logger::Info<problem::MultiObjective> &log_info, const problem::MetaData &pb_info) override
            {
                return any(log_info, pb_info);
            }
        };

        /** Create an `Any` trigger from a set of triggers.
         *
         * This function creates an `Any` trigger that combines the given triggers using a logical "or".
         * It is a convenient way to create an `Any` trigger without directly instantiating the class.
         *
         * @param triggers The triggers to be combined.
         * @return A reference to the created `Any` trigger.
         *
         * @ingroup Triggers
         */
        inline Any &any(logger::Triggers triggers)
        {
            const auto t = new Any(triggers);
            return *t;
        }

    } // namespace trigger
} // namespace ioh