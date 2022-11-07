#pragma once

#include "ioh/common/container_utils.hpp"
#include "ioh/common/factory.hpp"
#include "ioh/logger/analyzer_mo.hpp"
#include "ioh/problem/constraints.hpp"
#include "ioh/problem/problem.hpp"
#include "ioh/problem/structures.hpp"
#include "ioh/problem/utils.hpp"

namespace ioh
{
    namespace problem
    {
        /**
         * @brief Multi-objective Problem Base Class
         *
         * @tparam T type of the problem
         */
        template <typename T>
        class MultiObjectiveProblem 
        {
            /**
             * @brief Convienence typedef: shared_ptr of a Problem
             * @tparam T numeric type for the Problem
             */
            using ProblemPtr = std::shared_ptr<Problem<T>>;

        protected:
            //! Problem meta data
            MetaData meta_data_;

            //! The problem bounds
            Bounds<T> bounds_;

            //! The associated constraints constriants
            ConstraintSet<T> constraintset_; // TODO check interop with wrap problem

            //! The Problem state
            MultiObjectiveState<T> state_;

            //! The complete set of parent solutions
            std::vector<MultiObjectiveSolution<T>> optimum_;

            //! The set of problems_, pointing to the evaluate of each objective.
            std::vector<ProblemPtr> problems_;

            //! A pointer to the attached logger
            Logger *logger_{};

            //! The current log info
            logger::MultiObjectiveInfo log_info_;

            /**
             * @brief Method for checking the input given to the problem has the correct dimension
             *
             * @param x the input vector
             * @return true input is correct
             * @return false input is incorrect
             */
            [[nodiscard]] bool check_input_dimensions(const std::vector<T> &x)
            {
                if (x.empty())
                {
                    IOH_DBG(warning, "The solution is empty.")
                    return false;
                }
                if (x.size() != static_cast<size_t>(meta_data_.n_variables))
                {
                    IOH_DBG(warning, "The dimension of solution is incorrect.")
                    return false;
                }
                return true;
            }

            /**
             * @brief Check input given to problem for Integer types
             *
             * @tparam Integer integer type
             * @param x the input
             * @return true if correct
             */
            template <typename Integer = T>
            typename std::enable_if<std::is_integral<Integer>::value, bool>::type check_input(const std::vector<T> &x)
            {
                return check_input_dimensions(x);
            }

            /**
             * @brief Check input given to problem for Floating point types
             *
             * @tparam Floating floating type
             * @param x the input
             * @return true if correct
             */
            template <typename Floating = T>
            typename std::enable_if<std::is_floating_point<Floating>::value, bool>::type
            check_input(const std::vector<T> &x)
            {
                if (!check_input_dimensions(x))
                    return false;

                if (common::all_finite(x))
                    return true;

                if (common::has_nan(x))
                {
                    IOH_DBG(warning, "The solution contains NaN.")
                    return false;
                }
                if (common::has_inf(x))
                {
                    IOH_DBG(warning, "The solution contains Inf.")
                    return false;
                }
                IOH_DBG(warning, "The solution contains invalid values.")
                return false;
            }

        public:
            //! The current type held within the instance.
            using Type = T;

            /**
             * @brief Construct a new Problem object
             *
             * @param meta_data meta data for the problem
             * @param bounds the bounds to the problem
             * @param constraints a set of constraints for the problem
             * @param objective the solution to the problem
             */
            explicit MultiObjectiveProblem(MetaData meta_data, Bounds<T> bounds, ConstraintSet<T> constraints,
                                           MultiObjectiveSolution<T> objectives) :
                meta_data_(std::move(meta_data)),
                bounds_(std::move(bounds)), constraintset_(std::move(constraints)),
                state_(MultiObjectiveState<T>(MultiObjectiveSolution<T> (meta_data_.n_variables, meta_data_.n_objectives,meta_data_.optimization_type.type())))
            {
                bounds_.fit(meta_data_.n_variables);
                allocate_log_info();
            }

            /**
             * @brief Construct a new Problem object with an unkown solution
             *
             * @param meta_data meta data for the problem
             * @param bounds the bounds to the problem
             * @param constraints a set of constraint for the problem
             */
            explicit MultiObjectiveProblem(MetaData meta_data, Bounds<T> bounds = Bounds<T>(),
                                           ConstraintSet<T> constraints = {}) :
                MultiObjectiveProblem(
                    meta_data, bounds, constraints,
                    MultiObjectiveSolution<T>(meta_data.n_variables, meta_data.n_objectives, meta_data.optimization_type.type()))
            {
            }

            //! destructor
            virtual ~MultiObjectiveProblem() = default;

            //! Reset method, resets problem state and logger if attached
            virtual void reset()
            {
                state_.reset();
                    for (auto i = 0; i != problems_.size(); ++i) {
                        problems_[i]->reset();
                    }
                if (logger_ != nullptr)
                {
                    logger_->reset();
                    logger_->attach_problem(meta_data_);
                }
            }

            //! Update the current log info
            virtual void update_log_info()
            {
                log_info_.evaluations = static_cast<size_t>(state_.evaluations);

                // after transformation
                log_info_.transformed_y_mo = std::vector<double> (state_.y_unconstrained);
                log_info_.transformed_y_best_mo = std::vector<std::vector<double> >(state_.y_unconstrained_pareto.size());
                for (auto i = 0; i != state_.y_unconstrained_pareto.size(); ++i)
                {
                    log_info_.transformed_y_best_mo[i] = std::vector<double>(state_.y_unconstrained_pareto[i]);
                }

                // after constraint
                log_info_.y_mo = std::vector<double> (state_.current.y);
                log_info_.pareto_front = std::vector<problem::MultiObjectiveSolution <double> >(state_.current_pareto_front.size());
                for (auto i = 0; i != state_.current_pareto_front.size(); ++i)
                {
                    log_info_.pareto_front[i] = MultiObjectiveSolution<double>(std::vector<double>(state_.current_pareto_front[i].x.begin(),state_.current_pareto_front[i].x.end()),state_.current_pareto_front[i].y);
                }

                // constraint values
                log_info_.violations[0] = constraintset_.violation();
                log_info_.penalties[0] = constraintset_.penalty();

                for (size_t i = 0; i < constraintset_.n(); i++)
                {
                    log_info_.violations[i + 1] = constraintset_.constraints[i]->violation();
                    log_info_.penalties[i + 1] = constraintset_.constraints[i]->penalty();
                }

                log_info_.x = std::vector<double>(state_.current.x.begin(), state_.current.x.end());

                log_info_.has_improved = state_.has_improved;
            }

            //! Accessor for current log info
            [[nodiscard]] const logger::MultiObjectiveInfo &log_info() const { return log_info_; }

            //! Accessor for current log info
            void set_log_info(const logger::MultiObjectiveInfo &info) { log_info_ = info; }

            //! Attach a logger
            void attach_logger(Logger &logger)
            {
                logger_ = &logger;
                logger_->attach_problem(meta_data_);
            }

            //! Dettach a logger
            void detach_logger()
            {
                if (logger_ != nullptr)
                    logger_->reset();
                logger_ = nullptr;
            }

            //! Main call interface
            std::vector<double> operator()(const std::vector<T> &x)
            {
                if (!check_input(x))
                    return std::vector<double>(meta_data_.n_objectives,std::numeric_limits<double>::signaling_NaN());

                state_.current.x = x;
                if (constraintset_.hard_violation(x))
                {
                    state_.current.y = std::vector<double>(
                        meta_data_.n_objectives, constraintset_.penalize(meta_data_.optimization_type.initial_value()));
                }
                else
                {
                    state_.y_unconstrained = std::vector<double>(meta_data_.n_objectives);
                    state_.current.y = std::vector<double>(meta_data_.n_objectives);
                    size_t i = 0;
                    for (auto &p : problems_)
                    {
                        state_.y_unconstrained[i] = (*p)(x);
                        state_.current.y[i] = constraintset_.penalize(state_.y_unconstrained[i]);
                        ++i;
                    }
                    
                }

                state_.update(meta_data_, optimum_);

                if (logger_ != nullptr)
                {
                    update_log_info();
                    logger_->log(log_info());
                }

                return state_.current.y;
            }

            void enforce_bounds(const double weight = 1.0, const constraint::Enforced how = constraint::Enforced::SOFT,
                                const double exponent = 1.0)
            {

                bounds_.weight = weight;
                bounds_.exponent = exponent;
                bounds_.enforced = how;

                add_constraint(ConstraintPtr<T>(ConstraintPtr<T>(), &bounds_));
            }

            //! Accessor for `meta_data_`
            [[nodiscard]] MetaData meta_data() const { return meta_data_; }

            //! Accessor for `optimum_`
            [[nodiscard]] std::vector<MultiObjectiveSolution<T>> optimum() const { return optimum_; }

            //! Accessor for `state_`
            [[nodiscard]] MultiObjectiveState<T> state() const { return state_; }

            //! Accessor for `bounds_`
            [[nodiscard]] Bounds<T> bounds() { return bounds_; }

            //! Accessor for `constraintset_`
            [[nodiscard]] ConstraintSet<T> &constraints() { return constraintset_; }

            //! Alias for constraints().add
            void add_constraint(const ConstraintPtr<T> &c)
            {
                constraintset_.add(c);
                allocate_log_info();
            }

            //! Alias for constraints().remove
            void remove_constraint(const ConstraintPtr<T> &c)
            {
                constraintset_.remove(c);
                allocate_log_info();
            }

            //! Add an objective (a problem class)
            void add_objective(const ProblemPtr &p) 
            {
                problems_.push_back(p);
                meta_data_.n_objectives = problems_.size(); 
            }

            //! Stream operator
            friend std::ostream &operator<<(std::ostream &os, const MultiObjectiveProblem &obj)
            {
                return os << "Problem(\n\t" << obj.meta_data_ << "\n\t" << obj.constraintset_
                          << "\n\tstate: " << obj.state_;
            }

        private:
            void allocate_log_info()
            {
                log_info_.optimum_mo.resize(optimum_.size());
                for (auto i = 0; i != optimum_.size(); ++i)
                {
                    log_info_.optimum_mo[i].x = std::vector<double>(optimum_[i].x.begin(),optimum_[i].x.end());
                    log_info_.optimum_mo[i].y = std::vector<double>(optimum_[i].y);
                }
                log_info_.violations = std::vector<double>(constraintset_.n() + 1, 0.0);
                log_info_.penalties = std::vector<double>(constraintset_.n() + 1, 0.0);
            }
        };

        //! Type def for Real multiobjective problems
        using Real_MultiObjective = MultiObjectiveProblem<double>;

        //! Type def for Integer multiobjective problems
        using Integer_MultiObjective = MultiObjectiveProblem<int>;

    } // namespace problem

} // namespace ioh
