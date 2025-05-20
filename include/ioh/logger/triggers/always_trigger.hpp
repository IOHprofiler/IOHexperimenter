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
         * A trigger that always returns `true`.
         *
         * This trigger is used to ensure that logging or other actions
         * are performed on every call to the objective function, regardless
         * of the state or conditions.
         *
         * @ingroup Triggering
         */
        struct Always final : public logger::Trigger
        {
            /**
             * Always returns `true` for single-objective problems.
             *
             * @param info The logger information for the single-objective problem.
             * @param metadata The metadata of the problem.
             * @return Always returns `true`.
             */
            bool operator()(const logger::Info<problem::SingleObjective> &, const problem::MetaData &) override
            {
                IOH_DBG(debug, "always triggered")
                return true;
            }

            /**
             * Always returns `true` for multi-objective problems.
             *
             * @param info The logger information for the multi-objective problem.
             * @param metadata The metadata of the problem.
             * @return Always returns `true`.
             */
            bool operator()(const logger::Info<problem::MultiObjective> &, const problem::MetaData &) override
            {
                IOH_DBG(debug, "always triggered")
                return true;
            }
        };

        /** 
         * Do log at every call of the objective function.
         *
         * This instance of the `Always` trigger can be used directly
         * to ensure that logging is performed unconditionally.
         *
         * @ingroup Triggers
         */
        inline Always always; // Uncomment if one wants a library.

    } // namespace trigger
} // namespace ioh