#pragma once

#include "ioh/common/container_utils.hpp"
#include "ioh/common/factory.hpp"
#include "ioh/logger/loggers.hpp"
#include "ioh/problem/constraints.hpp"
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
         * @tparam R the returntype of the problem (evaluate)
         */
        template <typename T, typename R>
        class Problem
        {
        protected:
            //! Problem meta data
            MetaData meta_data_;

            //! The problem bounds
            Bounds<T> bounds_;

            //! The associated constraints constriants
            ConstraintSet<T> constraintset_; // TODO check interop with wrap problem

            //! The Problem state
            State<T, R> state_;

            //! The solution to the problem
            Solution<T, R> optimum_;

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
            [[nodiscard]] virtual R evaluate(const std::vector<T> &x) = 0;

            //! Variables transformation function
            [[nodiscard]] virtual std::vector<T> transform_variables(std::vector<T> x) { return x; }

            //! Objectives transformation function
            [[nodiscard]] virtual R transform_objectives(const R y) { return y; }

        public:
            //! The current type held within the instance.x
            using Type = T;

            //! The current ReturnType
            using ReturnType = R;

            //! Default constructor
            Problem(MetaData meta_data, Bounds<T> bounds, ConstraintSet<T> constraintset, State<T, R> state,
                    Solution<T, R> optimum) :
                meta_data_(std::move(meta_data)),
                bounds_(std::move(bounds)), constraintset_(std::move(constraintset)), state_(std::move(state)),
                optimum_(std::move(optimum))
            {
                bounds_.fit(meta_data_.n_variables);
                log_info_.allocate(optimum_, constraintset_);
            }

            //! Public call interface
            virtual R operator()(const std::vector<T> &x) = 0;

            virtual std::vector<R> operator()(const std::vector<std::vector<T>> &X) = 0;

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

            //! Accessor for current log info
            [[nodiscard]] const logger::Info &log_info() const { return log_info_; }

            //! Accessor for current log info
            void set_log_info(const logger::Info &info) { log_info_ = info; }

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
            /**
             * @brief Enforced the bounds(box-constraints) as constraint
             *
             * @param weight The weight for computing the penalty
             * @param how The constraint::Enforced strategy
             * @param exponent the exponent for scaling the contraint
             */
            void enforce_bounds(const double weight = 1.0,
                                const constraint::Enforced enforced = constraint::Enforced::SOFT,
                                const double exponent = 1.0)
            {
                for (auto &ci : constraintset_.constraints)
                {
                    auto ptr = std::dynamic_pointer_cast<Bounds<T>>(ci);
                    if (ptr && *ptr == bounds_)
                    {
                        remove_constraint(ptr);
                        break;
                    }
                }
                bounds_.enforced = enforced;
                bounds_.weight = weight;
                bounds_.exponent = exponent;
                add_constraint(std::make_shared<Bounds<T>>(bounds_.lb, bounds_.ub, bounds_.enforced, bounds_.weight,
                                                           bounds_.exponent));
            }

            //! Accessor for `meta_data_`
            [[nodiscard]] MetaData meta_data() const { return meta_data_; }

            //! Accessor for `optimum_`
            [[nodiscard]] Solution<T, R> optimum() const { return optimum_; }

            //! Accessor for `state_`
            [[nodiscard]] State<T, R> state() const { return state_; }

            //! Accessor for `bounds_`
            [[nodiscard]] Bounds<T> &bounds() { return bounds_; }

            //! Accessor for `constraintset_`
            [[nodiscard]] ConstraintSet<T> &constraints() { return constraintset_; }

            //! Alias for constraints().add
            void add_constraint(const ConstraintPtr<T> &c)
            {
                constraintset_.add(c);
                log_info_.allocate(optimum_, constraintset_);
            }

            //! Alias for constraints().remove
            void remove_constraint(const ConstraintPtr<T> &c)
            {
                constraintset_.remove(c);
                log_info_.allocate(optimum_, constraintset_);
            }

            //! Call this method after updating any fields on meta_data_
            void updated_metadata()
            {
                if (logger_ != nullptr)
                {
                    if (state_.evaluations != 0)
                        IOH_DBG(warning,
                                "Updated meta_data with logger attached and already evaluated problem. State will be "
                                "reset.")
                    reset();
                }
            }

            //! Accessor for problem id
            void set_id(const int new_id)
            {
                meta_data_.problem_id = new_id;
                updated_metadata();
            }

            //! Accessor for problem instance
            void set_instance(const int new_instance)
            {
                meta_data_.instance = new_instance;
                updated_metadata();
            }

            //! Accessor for problem name
            void set_name(const std::string &new_name)
            {
                meta_data_.name = new_name;
                updated_metadata();
            }

            //! Accessor for problem target
            void set_final_target(const double new_target)
            {
                meta_data_.final_target = new_target;
                updated_metadata();
            }

            //! Stream operator
            friend std::ostream &operator<<(std::ostream &os, const Problem &obj)
            {
                return os << "Problem(\n\t" << obj.meta_data_ << "\n\t" << obj.bounds_ << "\n\t" << obj.constraintset_
                          << "\n\t" << obj.state_ << "\n\t" << obj.optimum_ << "\n)";
            }
        };


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
         * @brief CRTP class for automatically registering problems in the apropriate factory.
         * you should inherit from this class when defining new real problems.
         * @tparam ProblemType The new problem type
         * @tparam ParentType The parent problem type
         */
        template <typename ProblemType, typename ParentType>
        struct RegisteredProblem : ParentType, AutomaticProblemRegistration<ProblemType, ParentType>
        {
            using ParentType::ParentType;
        };


        /**
         * @brief typedef for functions which can be wrapped in \ref SingleObjectiveWrappedProblem
         *
         * @tparam T type of the problem
         * @tparam R the returntype of the problem
         */
        template <typename T, typename R>
        using ObjectiveFunction = std::function<R(const std::vector<T> &)>;

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
         * @tparam R the returntype of the problem
         */
        template <typename R>
        using ObjectiveTransformationFunction = std::function<R(const double, const int)>;


        /**
         * @brief typedef for functions which compute a value for an optimum based on a given instance, dimension
         * combination Used in wrap_problem
         *
         * @tparam T type of the problem
         * @tparam R the returntype of the problem
         */
        template <typename T, typename R>
        using CalculateObjectiveFunction = std::function<Solution<T, R>(const int, const int)>;

    } // namespace problem
} // namespace ioh
