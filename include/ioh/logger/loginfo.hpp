#pragma once

#include <stdexcept>

#include "ioh/problem/structures.hpp"

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
            problem::Solution<double> optimum; 

            //! Single objective check whether state-update has caused an improvement
            bool has_improved;
        };
#ifdef _MSC_VER  
#pragma warning(default : 26495)
#endif
    } // log
} // ioh

