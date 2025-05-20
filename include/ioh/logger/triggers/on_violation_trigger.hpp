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
         * @brief A trigger that activates when there are constraint violations.
         * 
         * This trigger keeps track of the number of constraint violations and can be used
         * to log or take actions when violations occur.
         */
        struct OnViolation final : logger::Trigger {
            //! Track the number of violations
            int violations{}; ///< Counter for the number of violations

            /**
             * @brief Check for violations and update the counter.
             * 
             * @tparam R The type of the logger information (e.g., SingleObjective or MultiObjective).
             * @param log_info The logger information containing violation data.
             * @return True if there is a violation, false otherwise.
             */
            template<typename R>
            bool on_violations(const logger::Info<R> &log_info) {
                const bool violation = log_info.violations[0] != 0.;
                IOH_DBG(debug, "trigger OnViolation called: " << violation);
                violations += violation;
                return violation;
            }

            /**
             * @brief Call interface for SingleObjective problems.
             * 
             * @param log_info The logger information for the SingleObjective problem.
             * @param meta_data Metadata about the problem (unused in this implementation).
             * @return True if there is a violation, false otherwise.
             */
            bool operator()(const logger::Info<problem::SingleObjective> &log_info, const problem::MetaData&) override
            {
                return on_violations(log_info);
            }

            /**
             * @brief Call interface for MultiObjective problems.
             * 
             * @param log_info The logger information for the MultiObjective problem.
             * @param meta_data Metadata about the problem (unused in this implementation).
             * @return True if there is a violation, false otherwise.
             */
            bool operator()(const logger::Info<problem::MultiObjective> &log_info, const problem::MetaData&) override
            {
                return on_violations(log_info);
            }

            /**
             * @brief Reset the violations counter to zero.
             */
            void reset() override {
                violations = 0;
            }
        };

        //! Global instance of the OnViolation trigger
        inline OnViolation on_violation;

    } // namespace trigger
} // namespace ioh