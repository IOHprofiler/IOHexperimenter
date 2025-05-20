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
         * @brief Trigger that activates when the best objective function value improves by at least a specified delta.
         * 
         * This trigger is used to log events only when the improvement in the objective function value
         * exceeds a given threshold (delta). It supports both single-objective and multi-objective problems.
         */
        struct OnDeltaImprovement final : logger::Trigger {
            double delta;        ///< The minimum improvement required to trigger logging.
            double best_so_far;  ///< The best objective function value encountered so far.

            /**
             * @brief Constructor to initialize the trigger with a specified delta.
             * 
             * @param delta The minimum improvement required to trigger logging. Default is 1e-10.
             */
            explicit OnDeltaImprovement(const double delta = 1e-10): delta(delta) {
                reset();
            }

            /**
             * @brief Constructor to initialize the trigger with a specified delta and initial best value.
             * 
             * @param delta The minimum improvement required to trigger logging.
             * @param best_so_far The initial best objective function value.
             */
            OnDeltaImprovement(const double delta, const double best_so_far): delta(delta), best_so_far(best_so_far) {
            }
            
            /**
             * @brief Operator to evaluate the trigger for single-objective problems.
             * 
             * This operator checks if the current objective function value improves upon the best value
             * by at least the specified delta. If so, the trigger is activated.
             * 
             * @param log_info The logging information containing the current objective function value.
             * @param pb_info Metadata about the problem, including optimization type.
             * @return True if the trigger condition is met, otherwise false.
             */
            bool operator()(const logger::Info<problem::SingleObjective> &log_info, const problem::MetaData &pb_info) override {
                if (std::isnan(best_so_far)){
                    best_so_far = log_info.y;
                    return true;
                }

                if (pb_info.optimization_type(log_info.y, best_so_far) && std::abs(best_so_far - log_info.y) > delta) {
                    best_so_far = log_info.y;
                    return true;
                }                
                return false;
            }

            /**
             * @brief Operator to evaluate the trigger for multi-objective problems.
             * 
             * Currently, this implementation always returns true, as the logic for multi-objective
             * improvement is not yet implemented.
             * 
             * @param log_info The logging information for the multi-objective problem.
             * @param pb_info Metadata about the problem, including optimization type.
             * @return True (default behavior).
             */
            bool operator()(const logger::Info<problem::MultiObjective> &log_info, const problem::MetaData &pb_info) override {
                // Placeholder for multi-objective improvement logic
                // Uncomment and implement logic if needed in the future
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

            /**
             * @brief Reset the trigger state.
             * 
             * This method resets the best objective function value to NaN, indicating no value has been recorded yet.
             */
            void reset() override {
                best_so_far = std::numeric_limits<double>::signaling_NaN();
            }
        };

        /**
         * @brief Global instance of the OnDeltaImprovement trigger.
         * 
         * This instance can be used directly to create a trigger that logs only when the transformed
         * best objective function value found so far has strictly improved by a specified delta.
         * 
         * @ingroup Triggers
         */
        inline OnDeltaImprovement on_delta_improvement;

    } // namespace trigger
} // namespace ioh