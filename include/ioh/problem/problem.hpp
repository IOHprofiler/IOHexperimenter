#pragma once

#include "ioh/common/factory.hpp"
#include "ioh/common/container_utils.hpp"
#include "ioh/problem/structures.hpp"
#include "ioh/logger/loggers.hpp"

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
                    IOH_DBG(warning,"The solution is empty.")
                    return false;
                }
                if (x.size() != static_cast<size_t>(meta_data_.n_variables))
                {
                    IOH_DBG(warning,"The dimension of solution is incorrect.")
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
            typename std::enable_if<std::is_floating_point<Floating>::value, bool>::type check_input(
                const std::vector<T> &x)
            {
                if (!check_input_dimensions(x))
                    return false;

                if (common::all_finite(x))
                    return true;

                if (common::has_nan(x))
                {
                    IOH_DBG(warning,"The solution contains NaN.")
                    return false;
                }
                if (common::has_inf(x))
                {
                    IOH_DBG(warning,"The solution contains Inf.")
                    return false;
                }
                IOH_DBG(warning,"The solution contains invalid values.")
                return false;
            }
            
            //! Evaluation function
            [[nodiscard]] virtual double evaluate(const std::vector<T> &x) = 0;


            //! Variables transformation function
            [[nodiscard]]
            virtual std::vector<T> transform_variables(std::vector<T> x)
            {
                return x;
            }

            //! Objectives transformation function
            [[nodiscard]]
            virtual double transform_objectives(const double y)
            {
                return y;
            }

        public:
            /**
             * @brief Construct a new Problem object
             * 
             * @param meta_data meta data for the problem
             * @param constraint a constraint for the problem
             * @param objective the solution to the problem
             */
            explicit Problem(MetaData meta_data, Constraint<T> constraint, Solution<T> objective) :
                meta_data_(std::move(meta_data)), constraint_(std::move(constraint)),
                objective_(std::move(objective))
            {
                state_ = State<T>({std::vector<T>(meta_data_.n_variables, std::numeric_limits<T>::signaling_NaN()),
                                meta_data_.initial_objective_value});

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
            explicit Problem(MetaData meta_data, Constraint<T> constraint = Constraint<T>()):
                Problem(meta_data, constraint, {
                            std::vector<T>(meta_data.n_variables, std::numeric_limits<T>::signaling_NaN()),
                            meta_data.optimization_type == common::OptimizationType::Minimization
                            ? -std::numeric_limits<double>::infinity()
                            : std::numeric_limits<double>::infinity()
                        })
            {
            }

            //! destructor
            virtual ~Problem() = default;

            //! Reset method, resets problem state and logger if attached
            virtual void reset()
            {
                state_.reset();
                if (logger_ != nullptr) {
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
            [[nodiscard]] logger::Info &log_info()
            {
                return log_info_;
            }

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
            [[nodiscard]]
            MetaData meta_data() const
            {
                return meta_data_;
            }

            //! Accessor for `objective_`
            [[nodiscard]]
            Solution<T> objective() const
            {
                return objective_;
            }

            //! Accessor for `state_`
            [[nodiscard]]
            State<T> state() const
            {
                return state_;
            }

            //! Accessor for `constraint_`
            [[nodiscard]]
            Constraint<T> constraint() const
            {
                return constraint_;
            }

            //! Stream operator
            friend std::ostream &operator<<(std::ostream &os, const Problem &obj)
            {
                return os
                    << "Problem(\n\t" << obj.meta_data_
                    << "\n\tconstraint: " << obj.constraint_
                    << "\n\tstate: " << obj.state_
                    << "\n\tobjective: " << obj.objective_ << "\n)";
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
            //! Wrapped function
            ObjectiveFunction<T> function_;

            double evaluate(const std::vector<T> &x) override
            {
                return function_(x);
            }

        public:
            /**
             * @brief Construct a new Wrapped Problem object
             * 
             * @param f a function to be wrapped
             * @param name the name for the new function in the registry
             * @param n_variables the dimension of the problem
             * @param problem_id the problem id
             * @param optimization_type the type of optimization
             * @param constraint the contraint for the problem
             */
            WrappedProblem(ObjectiveFunction<T> f, const std::string &name, const int n_variables,
                           const int problem_id = 0, const int instance_id = 0,
                           const common::OptimizationType optimization_type = common::OptimizationType::Minimization,
                           Constraint<T> constraint = Constraint<T>()
                ) :
                Problem<T>(MetaData(problem_id, instance_id, name, n_variables, optimization_type), constraint),
                function_(f)
            {
            }
        };

        /**
         * @brief Shorthand for wrapping function in a problem 
         * 
         * @tparam T type of the problem
         * @param f a function to be wrapped
         * @param name the name for the new function in the registry
         * @param n_variables the dimension of the problem
         * @param optimization_type the type of optimization
         * @param constraint the contraint for the problem
         * @return WrappedProblem<T> A wrapped problem instance
         */
        template <typename T>
        WrappedProblem<T> wrap_function(ObjectiveFunction<T> f, const std::string &name, const int n_variables = 5,
                                        const common::OptimizationType optimization_type =
                                            common::OptimizationType::Minimization,
                                        Constraint<T> constraint = Constraint<T>()

                                        )
        {
            auto &factory = ProblemFactoryType<Problem<T>>::instance();

            int id = factory.check_or_get_next_available(1, name);

            factory.include(name, id, [=](const int, const int dimension)
            {
                return std::make_unique<WrappedProblem<T>>(f, name, dimension, id, optimization_type);
            });
            return WrappedProblem<T>{f, name, n_variables, id, optimization_type, constraint};
        }


        // template <typename T>
        // void wrap_function(
        //     ObjectiveFunction<T> f, 
        //     const std::string &name, 
        //     const common::OptimizationType optimization_type = common::OptimizationType::Minimization,
        //     const std::optional<T> lb = std::nullopt, const std::optional<T> ub = std::nullopt
        // )
        // {
        //     auto &factory = ProblemFactoryType<Problem<T>>::instance();

        //     int id = factory.check_or_get_next_available(1, name);

        //     auto constraint = Constraint<T>();

        //     factory.include(name, id, [=](const int instance, const int dimension)
        //     {
        //         return std::make_unique<WrappedProblem<T>>(f, name, dimension, id, instance, optimization_type, constraint);
        //     });
        // }

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
    }
}
