#pragma once

#include <utility>

#include "ioh/common.hpp"


namespace ioh {
    namespace logger {
        /**
         * \brief A observer class for \ref ioh::problem::base problems.
         * It contains five types of triggers, which determine when to log a event:
         *	1. Recording evaluations by a static interval.
         *	2. Recording all evaluations.
         *	3. Recording evaluations as the best found solution is updated.
         *	4. Recording evaluations at pre-defined points
         *	5. With a static number for each exponential bucket.
         */
        class Observer {
            /**
             * \brief variable for recording complete optimization process. i.e. every iteration is stored
             */
            bool trigger_always_;
            /**
             * \brief variable for recording when a better solution is found.
             */
            bool trigger_on_improvement_;
            /**
             * \brief variable for recording by a static interval.
             */
            int trigger_at_interval_;
            /**
             * \brief A vector of time points when to store data
             */
            std::vector<int> trigger_at_time_points_;

            /**
             * \brief The exponent used in conjunction with trigger_at_time_points_
             */
            int trigger_at_time_points_exp_base_;

            /**
             * \brief The number of triggers per time range
             */
            int triggers_per_time_range_;
            /**
             * \brief The exponent used in conjunction with triggers_per_time_range_
             */
            int triggers_per_time_range_exp_base_;

            /**
             * \brief The current best fitness value observed
             */
            double current_best_fitness_{};

            /**
             * \brief computes log(x)/log(b) and rounds to integer
             * \param evaluations the number of evaluations
             * \param exp the exponent
             * \return (int)log(x)/log(b)
             */
            [[nodiscard]] int compute_base(const size_t evaluations, const int exp) const {
                return static_cast<int>(
                    log(evaluations) / log(exp) + 1e-4 // add small const to help with rounding
                );
            }


        public:
            explicit Observer(
                const bool trigger_always = false,
                const int trigger_on_interval = 0,
                const int triggers_per_time_range = 0,
                const bool trigger_on_improvement = true,
                std::vector<int> trigger_at_time_points = {0},
                const common::OptimizationType optimization_type = common::OptimizationType::minimization,
                const int trigger_at_time_points_exp_base = 10,
                const int trigger_at_range_exp_base = 10
                )
                : trigger_always_(trigger_always),
                  trigger_on_improvement_(trigger_on_improvement),
                  trigger_at_interval_(trigger_on_interval),
                  trigger_at_time_points_(std::move(trigger_at_time_points)),
                  trigger_at_time_points_exp_base_(trigger_at_time_points_exp_base),
                  triggers_per_time_range_(triggers_per_time_range),
                  triggers_per_time_range_exp_base_(trigger_at_range_exp_base) {
                reset(optimization_type);
            }

            /**
             * \brief resets \ref current_best_fitness_
             * \param optimization_type Whether minimization or maximization is used
             */
            void reset(const common::OptimizationType optimization_type) {
                current_best_fitness_ = optimization_type == common::OptimizationType::maximization
                                            ? -std::numeric_limits<double>::infinity()
                                            : std::numeric_limits<double>::infinity();
            }

            /**
             * \brief returns true when trigger always is set to true
             * \return \ref trigger_always_
             */
            [[nodiscard]] bool trigger_always() const {
                return trigger_always_;
            }

            /**
             * \brief return true when a trigger at a static interval is defined
             * \return \ref trigger_at_interval_ != 0;
             */
            [[nodiscard]] bool trigger_at_interval() const {
                return trigger_at_interval_ != 0;
            }

            /**
             * \brief return true when a trigger on objective function improvement is defined
             * \return \ref trigger_on_improvement_
             */
            [[nodiscard]] bool trigger_on_improvement() const {
                return trigger_on_improvement_;
            }


            /**
             * \brief returns true when a trigger on specific time points is defined
             * \return true when \ref trigger_at_time_points_ is not empty
             */
            [[nodiscard]] bool trigger_at_time_points() const {
                return !trigger_at_time_points_.empty() && !(
                           trigger_at_time_points_.size() == 1 && trigger_at_time_points_[0] == 0);
            }

            /**
             * \brief returns true when a trigger on a specific time range is defined
             * \return \ref triggers_per_time_range_ > 0;
             */
            [[nodiscard]] bool trigger_at_time_range() const {
                return triggers_per_time_range_ > 0;
            }

            /**
             * \brief Checks whether the current number of evaluations is in the specified interval 
             * \param evaluations the current number of evaluations
             * \return true when the current number of evaluations is in the specified interval 
             */
            [[nodiscard]] bool
            interval_trigger(const size_t evaluations) const {
                return trigger_at_interval() && (evaluations == 1 || evaluations % trigger_at_interval_ == 0);
            }

            /**
             * \brief If \ref triggers_per_time_range_ is defined, It maintains that in each
             * [\ref triggers_per_time_range_exp_base_^base, \ref triggers_per_time_range_exp_base_^(base+1)]
             *  there will be \ref triggers_per_time_range_ triggers.
             * \param evaluations the current number of evaluations
             * \return true when the current evaluation should be logged
             */
            [[nodiscard]] bool time_range_trigger(const size_t evaluations) const {
                if (!trigger_at_time_range())
                    return false;

                const auto base = compute_base(evaluations, triggers_per_time_range_exp_base_);

                const auto start = static_cast<int>(std::pow(triggers_per_time_range_exp_base_, base));
                const auto stop = static_cast<int>(std::pow(triggers_per_time_range_exp_base_, base + 1));
                const auto interval = static_cast<int>(stop / triggers_per_time_range_);

                return  (evaluations - start) % interval == 0;
            }

            /**
             * \brief Check whether the current number of evaluations equals any defined in \ref trigger_at_time_points_
             * or an power of trigger_at_time_points_exp_base_ multiplied with any element of \ref trigger_at_time_points_.
             * For example, \ref trigger_at_time_points_ = {1,2,5} and \ref trigger_at_time_points_exp_base_ = 10;
             * the trigger returns true at 1, 2, 5, 10*1, 10*2, 10*5, 100*1, 100*2, 100*5,...
             * \param evaluations the current number of evaluations
             * \return true when the current evaluation should be logged
             */
            [[nodiscard]] bool time_points_trigger(const size_t evaluations) const {
                if (!trigger_at_time_points())
                    return false;

                const auto factor = std::pow(trigger_at_time_points_exp_base_,
                                        compute_base(evaluations, trigger_at_time_points_exp_base_));

                for (const auto &e : trigger_at_time_points_) {
                    if (evaluations == (e * factor))
                        return true;
                }
                return false;
            }

            /**
             * \brief Checks whether \ref fitness is an improvement on \ref current_best_fitness_ 
             * \param fitness The current fitness
             * \param optimization_type Whether minimization or maximization is used
             * \return true when fitness is better than \ref current_best_fitness_
             */
            bool improvement_trigger(
                const double fitness,
                const common::OptimizationType optimization_type = common::OptimizationType::minimization) {
                if (trigger_on_improvement_ && compare_objectives(fitness, current_best_fitness_, optimization_type)) {
                    current_best_fitness_ = fitness;
                    return true;
                }
                return false;
            }
        };

    }
}