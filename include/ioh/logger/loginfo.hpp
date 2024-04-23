#pragma once

#include <stdexcept>

#include "ioh/problem/structures.hpp"
#include "ioh/problem/constraints.hpp"

namespace ioh {

    /** Shared structure related to loggers. */
    namespace logger {

        /** Information about the current log.
         * 
         * @note The properties for bests values holds a state since the first start or the last call to reset.
         * 
         * @note "transformed" indicates that a monotonic transformation is applied,
         *       which is specific to the currently configured problem _instance_.
         * 
         * @note If there is an improvement, then fields holding "best" values will have the same value than the others.
         * 
         * @ingroup Logging
        */
#ifdef _MSC_VER
#pragma warning(disable : 26495)
#endif
        struct Info
        {
            //! Number of evaluations of the objective function so far.
            size_t evaluations;

            //! The current value
            double raw_y;

            //! The current best internal objective function value (since the last reset).
            double raw_y_best;

            //! The current transformed objective function value.
            double transformed_y;

            //! The current best transformed objective function value (since the last reset).
            double transformed_y_best;

            //! The current transformed objective function value with constraints applied.
            double y;

            //! The current best transformed objective function value with constraints applied (since the last reset).
            double y_best;

            //! Current search space variables
            std::vector<double> x;

            //! Constraint violations, first element is total violation by ContraintSet
            std::vector<double> violations;            

            //! Applied penalties by constraint, first element is total penalty applied by ContraintSet
            std::vector<double> penalties;
            
            //! Optimum to the current problem instance, with the corresponding transformed objective function value.
            problem::Solution<double, double> optimum; 

            //! Single objective check whether state-update has caused an improvement
            bool has_improved;


            /**
            * @brief update the log info based on the constraint and state of the problem
            * 
            * @tparam T the type of the problem
            * @param state the state of the problem
            * @param constraintset the set of constraints for the problem
            */
            template<typename T>
            void update(const problem::State<T, double>& state, const problem::ConstraintSet<T>& constraintset)
            {
                evaluations = static_cast<size_t>(state.evaluations);

                // before transformation
                raw_y = state.current_internal.y;
                raw_y_best = state.current_best_internal.y;

                // after transformation
                transformed_y = state.y_unconstrained;
                transformed_y_best = state.y_unconstrained_best;

                // after constraint
                y = state.current.y;
                y_best = state.current_best.y;

                // constraint values
                violations[0] = constraintset.violation();
                penalties[0] = constraintset.penalty();

                for (size_t i = 0; i < constraintset.n(); i++)
                {
                    violations[i + 1] = constraintset.constraints[i]->violation();
                    penalties[i + 1] = constraintset.constraints[i]->penalty();
                }

                x = std::vector<double>(state.current.x.begin(), state.current.x.end());

                has_improved = state.has_improved;    
            }

            /**
             * @brief allocate static (during a run) values for the log info
             *
             * @tparam T the type of the problem
             * @param opt the optimum of the problem
             * @param constraintset the set of constraints for the problem
             */
            template<typename T>
            void allocate(const problem::Solution<T, double> &opt, const problem::ConstraintSet<T> &constraintset)
            {
                optimum.x = std::vector<double>(opt.x.begin(), opt.x.end());
                optimum.y = opt.y;
                violations = std::vector<double>(constraintset.n() + 1, 0.0);
                penalties = std::vector<double>(constraintset.n() + 1, 0.0);
            }
        };
#ifdef _MSC_VER  
#pragma warning(default : 26495)
#endif
    } // log
} // ioh

