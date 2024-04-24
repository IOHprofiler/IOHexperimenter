#pragma once

#include "problem.hpp"

namespace ioh::problem
{
    /**
     * @brief CRTP class for Single objective problems
     *
     * @tparam T the type of the problem
     */
    template <typename T>
    class SingleObjectiveProblem : public Problem<T, SingleObjective>
    {
    protected:
        //! Inversion multiplier
        double inverter_ = 1.0;
    public:
        /**
         * @brief Construct a new Problem object
         *
         * @param meta_data meta data for the problem
         * @param bounds the bounds to the problem
         * @param constraints a set of constraints for the problem
         * @param optimum the solution to the problem
         */
        explicit SingleObjectiveProblem(MetaData meta_data, Bounds<T> bounds, ConstraintSet<T> constraints,
                                        Solution<T, SingleObjective> optimum) :
            Problem<T, SingleObjective>(
                meta_data, bounds, constraints,
                State<T, SingleObjective>(
                    {std::vector<T>(meta_data.n_variables, std::numeric_limits<T>::signaling_NaN()),
                     meta_data.optimization_type.initial_value()}),
                optimum)
        {
        }

        /**
         * @brief Construct a new Problem object with an unknown solution
         *
         * @param meta_data meta data for the problem
         * @param bounds the bounds to the problem
         * @param constraints a set of constraint for the problem
         */
        explicit SingleObjectiveProblem(MetaData meta_data, Bounds<T> bounds = Bounds<T>(),
                                        ConstraintSet<T> constraints = {}) :
            SingleObjectiveProblem(
                meta_data, bounds, constraints,
                Solution<T, SingleObjective>(meta_data.n_variables, meta_data.optimization_type.type()))
        {
        }

        void evaluate_for_state(const std::vector<T> &x, State<T, SingleObjective> &state)
        {
            state.current.x = x;
            if (this->constraintset_.hard_violation(x))
            {
                state.current_internal.x = x;
                state.current_internal.y =
                    this->constraintset_.penalize(this->meta_data_.optimization_type.initial_value());
                state.y_unconstrained = state.current_internal.y;
                state.current.y = state.current_internal.y;
            }
            else
            {
                state.current_internal.x = this->transform_variables(x);
                state.current_internal.y = this->evaluate(state.current_internal.x);
                state.y_unconstrained = this->transform_objectives(inverter_ * state.current_internal.y);
                state.current.y = this->constraintset_.penalize(state.y_unconstrained);
            }   
        }

        void update_state_and_log() {
            this->state_.update(this->meta_data_, this->optimum_);
            this->log_info_.update(this->state_, this->constraintset_);

            if (this->logger_ != nullptr)
            {
                this->logger_->log(this->log_info());
            }
        }


        //! Main call interface
        double operator()(const std::vector<T> &x) override
        {
            if (!this->check_input(x))
                return std::numeric_limits<double>::signaling_NaN();

            evaluate_for_state(x, this->state_);
            update_state_and_log();

            return this->state_.current.y;
        }

        //! Convert function from minimization to maximization or from maximization to minimization
        virtual void invert() {
            inverter_ *= -1.0;
            if (!std::isnan(this->optimum_.y))
                this->optimum_.y *= -1.0;

            this->state_.invert();
            if (this->meta_data_.optimization_type == common::OptimizationType::MAX){
                this->meta_data_.optimization_type = common::FOptimizationType(common::OptimizationType::MIN);
            } else{
                this->meta_data_.optimization_type = common::FOptimizationType(common::OptimizationType::MAX);   
            }
            this->reset();
        }

#if defined(_OPENMP)
        virtual std::vector<double> operator()(const std::vector<std::vector<T>> &X) override
        {

            const size_t n = X.size();
            std::vector<int> checked(n, 0);
            std::vector<State<T, SingleObjective>> states(n, this->state_);

            //------------------------------------ //
            //-----------[threaded code]---------- //
            //------------------------------------ //

            #pragma omp parallel for
            for (size_t i = 0; i < n; i++)
            {
                if (this->check_input(X[i])) {
                    evaluate_for_state(X[i], states[i]);
                    checked[i] = 1;
                }                
            }
            //------------------------------------ //

            std::vector<double> y(n);
            for (size_t i = 0; i < n; i++)
            {
                if(checked[i]) {
                    this->state_.current.x = states[i].current.x;
                    this->state_.current_internal.x = states[i].current_internal.x;
                    this->state_.current_internal.y = states[i].current_internal.y;
                    this->state_.y_unconstrained = states[i].y_unconstrained;
                    this->state_.current.y = states[i].current.y; 
                    update_state_and_log();
                    y[i] = states[i].current.y;
                } else{
                    y[i] = std::numeric_limits<double>::signaling_NaN();
                }
            }
            return y;
        }
#else
        virtual std::vector<double> operator()(const std::vector<std::vector<T>> &X) override
        {
            std::vector<double> y(X.size());
            for (size_t i = 0; i < y.size(); i++)
                y[i] = (*this)(X[i]);
            return y;
        }
#endif
    };

    //! Type def for Real problems
    using RealSingleObjective = SingleObjectiveProblem<double>;

    //! Type def for Integer problems
    using IntegerSingleObjective = SingleObjectiveProblem<int>;

    /**
     * @brief Wrapper class for new single objective problems
     *
     * @tparam T
     */
    template <typename T>
    class SingleObjectiveWrappedProblem final : public SingleObjectiveProblem<T>
    {
    protected:
        //! Wrapped objective function
        ObjectiveFunction<T, SingleObjective> function_;

        //! Wrapped variables transformation function
        VariablesTransformationFunction<T> transform_variables_function_;

        //! Wrapped objective transformation function
        ObjectiveTransformationFunction<SingleObjective> transform_objectives_function_;

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
         * @param bounds the box-constraints for the problem
         * @param transform_variables_function function which transforms the variables of the search problem
         * prior to calling f.
         * @param transform_objectives_function a function which transforms the objective value of the search
         * problem after calling f.
         * @param objective the value for the objective
         * @param constraints the constraints applied to the problem
         *
         */
        SingleObjectiveWrappedProblem(
            ObjectiveFunction<T, SingleObjective> f, const std::string &name, const int n_variables,
            const int problem_id = 0, const int instance_id = 0,
            const common::OptimizationType optimization_type = common::OptimizationType::MIN, Bounds<T> bounds = {},
            VariablesTransformationFunction<T> transform_variables_function = utils::identity<std::vector<T>, int>,
            ObjectiveTransformationFunction<SingleObjective> transform_objectives_function =
                utils::identity<double, int>,
            std::optional<Solution<T, double>> objective = std::nullopt, ConstraintSet<T> constraints = {}) :
            SingleObjectiveProblem<T>(MetaData(problem_id, instance_id, name, n_variables, optimization_type), bounds,
                                      constraints,
                                      objective.value_or(Solution<T, SingleObjective>(n_variables, optimization_type))),
            function_(f), transform_variables_function_(transform_variables_function),
            transform_objectives_function_(transform_objectives_function)
        {
        }
    };
} // namespace ioh::problem