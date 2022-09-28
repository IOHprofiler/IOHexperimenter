#pragma once

#include "ioh/common/container_utils.hpp"
#include "ioh/common/factory.hpp"
#include "ioh/logger/loggers.hpp"
#include "ioh/problem/structures.hpp"
#include "ioh/problem/constraints.hpp"
#include "ioh/problem/utils.hpp"

namespace ioh
{
    //! Problem namespace
    namespace problem
    {
        /**
         * @brief Problem Base class
         *
         * @tparam T type of the problem
         */
        template <typename T>
        class Problem
        {
        protected:
            //! Problem meta data
            MetaData meta_data_;

            //! The problem bounds
            Bounds<T> bounds_;            
            
            //! The associated constraints constriants 
            ConstraintSet<T> constraintset_; //TODO check interop with wrap problem

            //! The Problem state
            State<T> state_;

            //! The solution to the problem
            Solution<T> optimum_;

            //! A pointer to the attached logger
            Logger *logger_{};

            //! The current log info
            logger::Info log_info_;

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

            //! Evaluation function
            [[nodiscard]] virtual double evaluate(const std::vector<T> &x) = 0;

            //! Variables transformation function
            [[nodiscard]] virtual std::vector<T> transform_variables(std::vector<T> x) { return x; }

            //! Objectives transformation function
            [[nodiscard]] virtual double transform_objectives(const double y) { return y; }

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
            explicit Problem(MetaData meta_data, Bounds<T> bounds, ConstraintSet<T> constraints, Solution<T> objective) :
                meta_data_(std::move(meta_data)), bounds_(std::move(bounds)), constraintset_(std::move(constraints)),
                state_(State<T>({std::vector<T>(meta_data_.n_variables, std::numeric_limits<T>::signaling_NaN()),
                                 meta_data_.optimization_type.initial_value()})),
                optimum_(std::move(objective))
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
            explicit Problem(MetaData meta_data, Bounds<T> bounds = Bounds<T>(), ConstraintSet<T> constraints = {}) :
                Problem(meta_data, bounds, constraints,
                    Solution<T>(meta_data.n_variables, meta_data.optimization_type.type())
                )
            {
            }

            //! destructor
            virtual ~Problem() = default;

            //! Reset method, resets problem state and logger if attached
            virtual void reset()
            {
                state_.reset();
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
                
                // before transformation
                log_info_.raw_y = state_.current_internal.y;
                log_info_.raw_y_best = state_.current_best_internal.y; 

                // after transformation
                log_info_.transformed_y = state_.y_unconstrained;
                log_info_.transformed_y_best = state_.y_unconstrained_best;

                // after constraint
                log_info_.y = state_.current.y;
                log_info_.y_best = state_.current_best.y;

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
            [[nodiscard]] const logger::Info &log_info() const { return log_info_; }

            //! Accessor for current log info
            void set_log_info(const logger::Info& info) { log_info_ = info; }

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
            double operator()(const std::vector<T> &x)
            {
                if (!check_input(x))
                    return std::numeric_limits<double>::signaling_NaN();

                state_.current.x = x;
                if (constraintset_.hard_violation(x))
                {
                    state_.current_internal.x = x; 
                    state_.current_internal.y = constraintset_.penalize(meta_data_.optimization_type.initial_value());
                    state_.y_unconstrained = state_.current_internal.y;
                    state_.current.y = state_.current_internal.y;
                }
                else
                {
                    state_.current_internal.x = transform_variables(x);
                    state_.current_internal.y = evaluate(state_.current_internal.x);
                    state_.y_unconstrained = transform_objectives(state_.current_internal.y);
                    state_.current.y = constraintset_.penalize(state_.y_unconstrained);
                }
                
                state_.update(meta_data_, optimum_);

                if (logger_ != nullptr)
                {
                    update_log_info();
                    logger_->log(log_info());
                }

                return state_.current.y;
            }

            void enforce_bounds(const double weight = 1.0, const constraint::Enforced how = constraint::Enforced::SOFT, const double exponent = 1.0)
            {
                
                bounds_.weight = weight;
                bounds_.exponent = exponent;
                bounds_.enforced = how;
                
                add_constraint(ConstraintPtr<T>(ConstraintPtr<T>(), &bounds_));
            }          

            //! Accessor for `meta_data_`
            [[nodiscard]] MetaData meta_data() const { return meta_data_; }

            //! Accessor for `optimum_`
            [[nodiscard]] Solution<T> optimum() const { return optimum_; }

            //! Accessor for `state_`
            [[nodiscard]] State<T> state() const { return state_; }

            //! Accessor for `bounds_`
            [[nodiscard]] Bounds<T> bounds() { return bounds_; }

            //! Accessor for `constraintset_`
            [[nodiscard]] ConstraintSet<T>& constraints() { return constraintset_; }    

            //! Alias for constraints().add
            void add_constraint(const ConstraintPtr<T> &c) { 
                constraintset_.add(c);
                allocate_log_info();
            }

            //! Alias for constraints().remove
            void remove_constraint(const ConstraintPtr<T> &c)
            {
                constraintset_.remove(c);
                allocate_log_info();
            }

            //! Stream operator
            friend std::ostream &operator<<(std::ostream &os, const Problem &obj)
            {
                return os << "Problem(\n\t" << obj.meta_data_ << "\n\t" << obj.constraintset_
                          << "\n\tstate: " << obj.state_ << "\n\toptimum: " << obj.optimum_ << "\n)";
            }

        private:
            void allocate_log_info() {
                log_info_.optimum.x = std::vector<double>(optimum_.x.begin(), optimum_.x.end());
                log_info_.optimum.y = optimum_.y;
                log_info_.violations = std::vector<double>(constraintset_.n() + 1, 0.0);
                log_info_.penalties = std::vector<double>(constraintset_.n() + 1, 0.0);
            }
        };

        /**
         * @brief typedef for functions which can be wrapped in \ref WrappedProblem
         *
         * @tparam T type of the problem
         */
        template <typename T>
        using ObjectiveFunction = std::function<double(const std::vector<T> &)>;

        /**
         * @brief typedef for functions which take a vector and return a transformed version of that vector.
         * Used in WrappedProblem
         *
         * @tparam T type of the problem
         */
        template <typename T>
        using VariablesTransformationFunction = std::function<std::vector<T>(std::vector<T>, const int)>;

        /**
         * @brief typedef for functions which take a value and return a transformed version of that value.
         * Used in WrappedProblem
         *
         * @tparam T type of the problem
         */
        using ObjectiveTransformationFunction = std::function<double(const double, const int)>;


        /**
         * @brief typedef for functions which compute a value for an optimum based on a given instance, dimension
         * combination Used in wrap_problem
         *
         * @tparam T type of the problem
         */
        template <typename T>
        using CalculateObjectiveFunction = std::function<Solution<T>(const int, const int)>;

        /**
         * @brief Registry type def
         *
         * @tparam ProblemType type of the problem
         */
        template <typename ProblemType>
        using ProblemRegistryType = common::RegisterWithFactory<ProblemType, int, int>;

        /**
         * @brief Factory type def
         *
         * @tparam ProblemType type of the problem
         */
        template <typename ProblemType>
        using ProblemFactoryType = common::Factory<ProblemType, int, int>;


        /**
         * @brief Automatic Factory registration type def.
         *
         * @tparam Derived type of the problem
         * @tparam Parent type of the problem
         */
        template <class Derived, class Parent>
        using AutomaticProblemRegistration = common::AutomaticTypeRegistration<Derived, ProblemRegistryType<Parent>>;

        /**
         * @brief Registry type def.
         *
         * @tparam Parent type of the problem
         */
        template <class Parent>
        using ProblemRegistry = ProblemRegistryType<Parent>;

        /**
         * @brief
         *
         * @tparam T
         */
        template <typename T>
        class WrappedProblem final : public Problem<T>
        {
        protected:
            //! Wrapped objective function
            ObjectiveFunction<T> function_;

            //! Wrapped variables transformation function
            VariablesTransformationFunction<T> transform_variables_function_;

            //! Wrapped objective transformation function
            ObjectiveTransformationFunction transform_objectives_function_;

        protected:
            //! Pass call to wrapped function
            double evaluate(const std::vector<T> &x) override { return function_(x); }

            //! Variables transformation function
            std::vector<T> transform_variables(std::vector<T> x) override
            {
                return transform_variables_function_(x, this->meta_data_.instance);
            }

            //! Objectives transformation function
            double transform_objectives(const double y) override
            {
                return transform_objectives_function_(y, this->meta_data_.instance);
            }

        public:
            /**
             * @brief Construct a new Wrapped Problem object
             *
             * @param f a function to be wrapped
             * @param name the name for the new function in the registry
             * @param n_variables the dimension of the problem
             * @param problem_id the problem id
             * @param instance_id the problem instance
             * @param optimization_type the type of optimization
             * @param BoxConstraint the contraint for the problem
             * @param transform_variables_function function which transforms the variables of the search problem
             * prior to calling f.
             * @param transform_objectives_function a function which transforms the objective value of the search
             * problem after calling f.
             * @param objective the value for the objective
             * @param constraints the constraints applied to the problem
             *
             */
            WrappedProblem(
                ObjectiveFunction<T> f, const std::string &name, const int n_variables, const int problem_id = 0,
                const int instance_id = 0,
                const common::OptimizationType optimization_type = common::OptimizationType::MIN,
                Bounds<T> bounds = {}, 
                VariablesTransformationFunction<T> transform_variables_function = utils::identity<std::vector<T>, int>,
                ObjectiveTransformationFunction transform_objectives_function = utils::identity<double, int>,
                std::optional<Solution<T>> objective = std::nullopt, ConstraintSet<T> constraints = {}
                ) :
                Problem<T>(
                    MetaData(problem_id, instance_id, name, n_variables, optimization_type), 
                    bounds,                      
                    constraints,
                    objective.value_or(Solution<T>(n_variables, optimization_type))
                ),
                function_(f), transform_variables_function_(transform_variables_function),
                transform_objectives_function_(transform_objectives_function)
            {
            }
        };

        /**
         * @brief Shorthand for wrapping function in a problem.
         *
         * @tparam T type of the problem
         * @param f a function to be wrapped
         * @param name the name for the new function in the registry
         * @param optimization_type the type of optimization
         * @param lb lower bound for the BoxConstraint of the problem
         * @param ub upper bound for the BoxConstraint of the problem
         * @param transform_variables_function function which transforms the variables of the search problem
         * prior to calling f.
         * @param transform_objectives_function a function which transforms the objective value of the search problem
         * after calling f.
         * @param calculate_optimum a function which returns the optimum based on a given problem
         * dimension and instance.
         * @param constraints the constraints
         */
        template <typename T>
        void
        wrap_function(ObjectiveFunction<T> f, const std::string &name,
                      const common::OptimizationType optimization_type = common::OptimizationType::MIN,
                      const std::optional<T> lb = std::nullopt, const std::optional<T> ub = std::nullopt,
                      std::optional<VariablesTransformationFunction<T>> transform_variables_function = std::nullopt,
                      std::optional<ObjectiveTransformationFunction> transform_objectives_function = std::nullopt,
                      std::optional<CalculateObjectiveFunction<T>> calculate_optimum = std::nullopt,
                      ConstraintSet<T> constraints = {}
            )
        {
            auto &factory = ProblemFactoryType<Problem<T>>::instance();

            int id = factory.check_or_get_next_available(1, name);

            auto bounds = BoxConstraint<T>(
                1, lb.value_or(std::numeric_limits<T>::lowest()), ub.value_or(std::numeric_limits<T>::max())
            );

            auto tx = transform_variables_function.value_or(utils::identity<std::vector<T>, int>);
            auto ty = transform_objectives_function.value_or(utils::identity<double, int>);

            factory.include(name, id,
                            [f, name, id, optimization_type, bounds, tx, ty, calculate_optimum, constraints](const int iid,
                                                                                                      const int dim) {
                                auto optimum = calculate_optimum ? calculate_optimum.value()(iid, dim)
                                                                     : Solution<T>(dim, optimization_type);

                                return std::make_unique<WrappedProblem<T>>(f, name, dim, id, iid, optimization_type,
                                                                           bounds, tx, ty, optimum, constraints);
                            });
        }

        //! Type def for Real problems
        using Real = Problem<double>;

        //! Type def for Integer problems
        using Integer = Problem<int>;

        /**
         * @brief CRTP class for automatically registering problems in the apropriate factory.
         * you should inherit from this class when defining new real problems.
         * @tparam ProblemType The new problem type
         */
        template <typename ProblemType>
        class RealProblem : public Real, AutomaticProblemRegistration<ProblemType, Real>
        {
        public:
            using Real::Real;
        };

        /**
         * @brief CRTP class for automatically registering problems in the apropriate factory.
         * you should inherit from this class when defining new integer problems.
         * @tparam ProblemType The new problem type
         */
        template <typename ProblemType>
        struct IntegerProblem : Integer, AutomaticProblemRegistration<ProblemType, Integer>
        {
            using Integer::Integer;
        };
    } // namespace problem
} // namespace ioh
