#pragma once

#include "ioh/common/container_utils.hpp"
#include "ioh/common/factory.hpp"
#include "ioh/logger/loggers.hpp"
#include "ioh/problem/structures.hpp"
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

            //! The Constraint
            Constraint<T> constraint_;

            //! The Problem state
            State<T> state_;

            //! The solution to the problem
            Solution<T> objective_;

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

            //! TODO: check why this is not const
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
             * @param constraint a constraint for the problem
             * @param objective the solution to the problem
             */
            explicit Problem(MetaData meta_data, Constraint<T> constraint, Solution<T> objective) :
                meta_data_(std::move(meta_data)), constraint_(std::move(constraint)),
                state_(State<T>({std::vector<T>(meta_data.n_variables, std::numeric_limits<T>::signaling_NaN()),
                                 meta_data.initial_objective_value})),
                objective_(std::move(objective))
            {
                constraint_.check_size(meta_data_.n_variables);
                log_info_.optimum = objective_.as_double();
                log_info_.current = state_.current.as_double();
            }

            /**
             * @brief Construct a new Problem object with an unkown solution
             *
             * @param meta_data meta data for the problem
             * @param constraint a constraint for the problem
             */
            explicit Problem(MetaData meta_data, Constraint<T> constraint = Constraint<T>()) :
                Problem(meta_data, constraint, {meta_data.n_variables, meta_data.optimization_type.type()})
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
                log_info_.raw_y_best = state_.current_best_internal.y;
                log_info_.transformed_y = state_.current.y;
                log_info_.transformed_y_best = state_.current_best.y;
                log_info_.current = state_.current.as_double();
            }

            //! Accessor for current log info
            [[nodiscard]] logger::Info &log_info() { return log_info_; }

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
                state_.current_internal.x = transform_variables(x);
                state_.current_internal.y = evaluate(state_.current_internal.x);
                state_.current.y = transform_objectives(state_.current_internal.y);
                state_.update(meta_data_, objective_);
                if (logger_ != nullptr)
                {
                    update_log_info();
                    logger_->log(log_info());
                }
                return state_.current.y;
            }

            //! Accessor for `meta_data_`
            [[nodiscard]] MetaData meta_data() const { return meta_data_; }

            //! Accessor for `objective_`
            [[nodiscard]] Solution<T> objective() const { return objective_; }

            //! Accessor for `state_`
            [[nodiscard]] State<T> state() const { return state_; }

            //! Accessor for `constraint_`
            [[nodiscard]] Constraint<T> constraint() const { return constraint_; }

            //! Stream operator
            friend std::ostream &operator<<(std::ostream &os, const Problem &obj)
            {
                return os << "Problem(\n\t" << obj.meta_data_ << "\n\tconstraint: " << obj.constraint_
                          << "\n\tstate: " << obj.state_ << "\n\tobjective: " << obj.objective_ << "\n)";
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
             * @param constraint the contraint for the problem
             * @param transform_variables_function function which transforms the variables of the search problem
             * prior to calling f.
             * @param transform_objectives_function a function which transforms the objective value of the search
             * problem after calling f.
             * @param objective the value for the objective
             *
             */
            WrappedProblem(
                ObjectiveFunction<T> f, const std::string &name, const int n_variables, const int problem_id = 0,
                const int instance_id = 0,
                const common::OptimizationType optimization_type = common::OptimizationType::Minimization,
                Constraint<T> constraint = Constraint<T>(),
                VariablesTransformationFunction<T> transform_variables_function = utils::identity<std::vector<T>, int>,
                ObjectiveTransformationFunction transform_objectives_function = utils::identity<double, int>,
                std::optional<Solution<T>> objective = std::nullopt) :
                Problem<T>(MetaData(problem_id, instance_id, name, n_variables, optimization_type), constraint,
                           objective.value_or(Solution<T>(n_variables, optimization_type))),
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
         * @param lb lower bound for the constraint of the problem
         * @param ub upper bound for the constraint of the problem
         * @param transform_variables_function function which transforms the variables of the search problem
         * prior to calling f.
         * @param transform_objectives_function a function which transforms the objective value of the search problem
         * after calling f.
         * @param calculate_objective a function which returns the optimum based on a given problem
         * dimension and instance.
         */
        template <typename T>
        void
        wrap_function(ObjectiveFunction<T> f, const std::string &name,
                      const common::OptimizationType optimization_type = common::OptimizationType::Minimization,
                      const std::optional<T> lb = std::nullopt, const std::optional<T> ub = std::nullopt,
                      std::optional<VariablesTransformationFunction<T>> transform_variables_function = std::nullopt,
                      std::optional<ObjectiveTransformationFunction> transform_objectives_function = std::nullopt,
                      std::optional<CalculateObjectiveFunction<T>> calculate_objective = std::nullopt)
        {
            auto &factory = ProblemFactoryType<Problem<T>>::instance();

            int id = factory.check_or_get_next_available(1, name);

            auto constraint = Constraint<T>(1, lb.value_or(std::numeric_limits<T>::lowest()),
                                            ub.value_or(std::numeric_limits<T>::max()));

            auto tx = transform_variables_function.value_or(utils::identity<std::vector<T>, int>);
            auto ty = transform_objectives_function.value_or(utils::identity<double, int>);

            factory.include(name, id,
                            [f, name, id, optimization_type, constraint, tx, ty, calculate_objective](const int iid,
                                                                                                      const int dim) {
                                auto objective = calculate_objective ? calculate_objective.value()(iid, dim)
                                                                     : Solution<T>(dim, optimization_type);

                                return std::make_unique<WrappedProblem<T>>(f, name, dim, id, iid, optimization_type,
                                                                           constraint, tx, ty, objective);
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
