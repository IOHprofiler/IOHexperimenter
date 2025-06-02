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
    class MultiObjectiveProblem : public Problem<T, MultiObjective>
    {
    protected:
        //! Inversion multiplier
        double inverter_ = 1.0;
    public:

        /**
         * @brief Construct a new Problem object with an unknown solution
         *
         * @param meta_data meta data for the problem
         * @param bounds the bounds to the problem
         * @param constraints a set of constraint for the problem
         */
        explicit MultiObjectiveProblem(
                MetaData meta_data, 
                Bounds<T> bounds,
                ConstraintSet<T> constraints,
                Solution<T, MultiObjective> optimum
            ) :
            Problem<T,MultiObjective>(
                meta_data, 
                bounds, 
                constraints,
                State<T, MultiObjective>(Solution<T, MultiObjective>(
                    std::vector<T>(meta_data.n_variables, std::numeric_limits<T>::signaling_NaN()),
                    std::vector<double>(meta_data.n_objectives, meta_data.optimization_type.initial_value())
                )),
                optimum
            )
        {
        }

        explicit MultiObjectiveProblem(
                MetaData meta_data, 
                Bounds<T> bounds = Bounds<T>(),
                ConstraintSet<T> constraints = {}
            ) :
            MultiObjectiveProblem(
                meta_data, 
                bounds, 
                constraints,
                Solution<T, MultiObjective>(
                    meta_data.n_variables, 
                    meta_data.n_objectives, 
                    meta_data.optimization_type.type()
                )
            )
        {
        }


        void evaluate_for_state(const std::vector<T> &x, State<T, MultiObjective> &state)
        {

            state.current.x = x;
            if (this->constraintset_.hard_violation(x))
            {
                for (size_t i = 0; i < this->meta_data_.n_objectives; ++i)
                {
                    state.current.y[i] =
                        this->constraintset_.penalize(this->meta_data_.optimization_type.initial_value());
                }
            }
            else
            {
                state.current.x = this->transform_variables(x);
                auto y = this->evaluate(state.current.x);
                auto y_unconstrained = this->transform_objectives(y);
                for (size_t i = 0; i < y_unconstrained.size(); ++i)
                {
                    y_unconstrained[i] *= inverter_;
                }
                state.current.y = y_unconstrained;
                for (size_t i = 0; i < state.current.y.size(); ++i)
                {
                    state.current.y[i] = this->constraintset_.penalize(y_unconstrained[i]);
                }
                
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
        std::vector<double> operator()(const std::vector<T> &x) override
        {
            if (!this->check_input(x))
                return std::vector<double>(this->meta_data_.n_objectives, std::numeric_limits<double>::signaling_NaN());
            evaluate_for_state(x, this->state_);
            update_state_and_log();

            return this->state_.current.y;
        }

        //! Convert function from minimization to maximization or from maximization to minimization
        virtual void invert() {
            inverter_ *= -1.0;
            if (this->optimum_.exists())
                for (auto &value : this->optimum_.y)
                {
                    value *= -1.0;
                }

            this->state_.invert();
            if (this->meta_data_.optimization_type == common::OptimizationType::MAX){
                this->meta_data_.optimization_type = common::FOptimizationType(common::OptimizationType::MIN);
            } else{
                this->meta_data_.optimization_type = common::FOptimizationType(common::OptimizationType::MAX);   
            }
            this->reset();
        }

#if defined(_OPENMP)
        virtual std::vector<std::vector<double>> operator()(const std::vector<std::vector<T>> &X) override
        {
            std::vector<std::vector<double>> y(X.size());
            for (size_t i = 0; i < y.size(); i++)
                y[i] = (*this)(X[i]);
            return y;
            // const size_t n = X.size();
            // std::vector<int> checked(n, 0);
            // std::vector<State<T, SingleObjective>> states(n, this->state_);

            // //------------------------------------ //
            // //-----------[threaded code]---------- //
            // //------------------------------------ //

            // #pragma omp parallel for
            // for (size_t i = 0; i < n; i++)
            // {
            //     if (this->check_input(X[i])) {
            //         evaluate_for_state(X[i], states[i]);
            //         checked[i] = 1;
            //     }                
            // }
            // //------------------------------------ //

            // std::vector<double> y(n);
            // for (size_t i = 0; i < n; i++)
            // {
            //     if(checked[i]) {
            //         this->state_.current.x = states[i].current.x;
            //         this->state_.current.x = states[i].current.x;
            //         this->state_.current.y = states[i].current.y;
            //         this->state_.y_unconstrained = states[i].y_unconstrained;
            //         this->state_.current.y = states[i].current.y; 
            //         update_state_and_log();
            //         y[i] = states[i].current.y;
            //     } else{
            //         y[i] = std::numeric_limits<double>::signaling_NaN();
            //     }
            // }
            // return y;
        }
#else
        virtual std::vector<std::vector<double>> operator()(const std::vector<std::vector<T>> &X) override
        {
            std::vector<std::vector<double>> y(X.size());
            for (size_t i = 0; i < y.size(); i++)
                y[i] = (*this)(X[i]);
            return y;
        }
#endif
    };

    //! Type def for Real problems
    using RealMultiObjective = MultiObjectiveProblem<double>;

    //! Type def for Integer problems
    using IntegerMultiObjective = MultiObjectiveProblem<int>;


    /**
     * @brief Wrapper class for new single objective problems
     *
     * @tparam T
     */
    template <typename T>
    class MultiObjectiveWrappedProblem final : public MultiObjectiveProblem<T>
    {
    protected:
        //! Wrapped objective function
        ObjectiveFunction<T, MultiObjective> function_;

        //! Wrapped variables transformation function
        VariablesTransformationFunction<T> transform_variables_function_;

        //! Wrapped objective transformation function
        ObjectiveTransformationFunction<MultiObjective> transform_objectives_function_;

    protected:
        //! Pass call to wrapped function
        std::vector<double> evaluate(const std::vector<T> &x) override { return function_(x); }

        //! Variables transformation function
        std::vector<T> transform_variables(std::vector<T> x) override
        {
            return transform_variables_function_(x, this->meta_data_.instance);
        }

        //! Objectives transformation function
        std::vector<double> transform_objectives(const std::vector<double> y) override
        {
            return transform_objectives_function_(y, this->meta_data_.instance);
        }

    public:
        MultiObjectiveWrappedProblem(
            ObjectiveFunction<T, MultiObjective> f, 
            const std::string &name, 
            const int n_variables,
            const int n_objectives, 
            const int problem_id = 0, 
            const int instance_id = 0,
            const common::OptimizationType optimization_type = common::OptimizationType::MIN, 
            Bounds<T> bounds = {},
            VariablesTransformationFunction<T> transform_variables_function = utils::identity<std::vector<T>, int>,
            ObjectiveTransformationFunction<MultiObjective> transform_objectives_function =utils::identity<std::vector<double>, int>,
            std::optional<Solution<T, MultiObjective>> objective = std::nullopt, 
            ConstraintSet<T> constraints = {}
        ) :
            MultiObjectiveProblem<T>(
                MetaData(problem_id, instance_id, name, n_variables, n_objectives, optimization_type), 
                bounds,
                constraints,
                objective.value_or(
                    Solution<T, MultiObjective>(
                        n_variables, 
                        n_objectives, 
                        optimization_type
                    )
                )
                ),
            function_(f), 
            transform_variables_function_(transform_variables_function),
            transform_objectives_function_(transform_objectives_function)
        {
        }
    };
} // namespace ioh::problem