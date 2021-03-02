#pragma once

#include "ioh/common.hpp"
#include "ioh/logger/base.hpp"


namespace ioh
{
    namespace problem
    {
        template <typename T>
        struct Solution
        {
            std::vector<T> x;
            std::vector<double> y;

            friend std::ostream &operator<<(std::ostream &os, const Solution &obj)
            {
                return os
                    << "x: " << common::vector_to_string(obj.x)
                    << " y: " << common::vector_to_string(obj.y);
            }
        };

        template <typename T>
        struct Constraint
        {
            std::vector<T> ub;
            std::vector<T> lb;

            Constraint(const std::vector<T> upper, const std::vector<T> lower) :
                ub(upper), lb(lower)
            {
            }

            explicit Constraint(const int size = 1,
                                const T upper = std::numeric_limits<T>::max(),
                                const T lower = std::numeric_limits<T>::lowest()
                ) :
                Constraint(std::vector<T>(size, upper), std::vector<T>(size, lower))
            {
            }

            void check_size(const int s)
            {
                if (ub.size() == lb.size() == size_t{1})
                {
                    ub = std::vector<T>(s, ub.at(0));
                    lb = std::vector<T>(s, lb.at(0));
                }

                if ((ub.size() != static_cast<size_t>(s)) || (ub.size() != lb.size()))
                    std::cout << "Bound dimension is wrong" << std::endl;
            }

            bool check(const std::vector<T> &x)
            {
                for (auto i = 0; i < x.size(); i++)
                    if (!(lb.at(i) >= x.at(i) <= lb.at(i)))
                        return false;
                return true;
            }

            friend std::ostream &operator<<(std::ostream &os, const Constraint &obj)
            {
                os << "[ ";
                for (auto i = 0; i < obj.ub.size(); i++)
                    os << i << ": (" << obj.lb.at(i) << ", " << obj.ub.at(i) << ") ";
                os << "]";
                return os;
            }
        };

        struct MetaData
        {
            int instance{};
            int problem_id{};
            std::string name;
            common::OptimizationType optimization_type;
            int n_variables{};
            int n_objectives{};
            double initial_objective_value{};

            MetaData(const int problem_id, const int instance, std::string name, const int n_variables,
                     const int n_objectives,
                     const common::OptimizationType optimization_type = common::OptimizationType::minimization
                ) :
                instance(instance),
                problem_id(problem_id),
                name(std::move(name)),
                optimization_type(optimization_type),
                n_variables(n_variables),
                n_objectives(n_objectives),
                initial_objective_value(optimization_type == common::OptimizationType::minimization
                    ? std::numeric_limits<double>::infinity()
                    : -std::numeric_limits<double>::infinity())
            {
            }

            MetaData(const int instance, const std::string &name, const int n_variables, const int n_objectives = 1,
                     const common::OptimizationType optimization_type = common::OptimizationType::minimization
                ) :
                MetaData(0, instance, name, n_variables, n_objectives, optimization_type)
            {
            }

            friend std::ostream &operator<<(std::ostream &os, const MetaData &obj)
            {
                os << obj.name;
                if (obj.problem_id != 0)
                    os << " id: " << obj.problem_id;
                if (obj.instance != 0)
                    os << " instance: " << obj.instance;

                return os
                    << " optimization_type: " << (obj.optimization_type == common::OptimizationType::minimization
                        ? "minimization"
                        : "maximization")
                    << " n_variables: " << obj.n_variables
                    << " n_objectives: " << obj.n_objectives;
            }
        };

        template <typename T>
        struct State
        {
        private:
            Solution<T> initial_solution;
        public:
            int evaluations = 0;
            bool optimum_found = false;
            Solution<T> current_best_internal;
            Solution<T> current_best;

            Solution<T> current_internal;
            Solution<T> current;

            State() = default;

            State(Solution<T> initial_solution) :
                initial_solution(std::move(initial_solution))
            {
                reset();
            }

            void reset()
            {
                evaluations = 0;
                current_best = initial_solution;
                current_best_internal = initial_solution;
                optimum_found = false;
            }

            void update(const MetaData &meta_data, const Solution<T> &objective)
            {
                ++evaluations;
                if (common::compare_objectives(current.y, current_best.y, meta_data.optimization_type))
                {
                    current_best_internal = current_internal;
                    current_best = current;

                    if (common::compare_vector(objective.y, current.y))
                        optimum_found = true;
                }
            }

            friend std::ostream &operator<<(std::ostream &os, const State &obj)
            {
                return os
                    << "evaluations: " << obj.evaluations
                    << " optimum_found: " << std::boolalpha << obj.optimum_found
                    << " current_best: " << obj.current_best;
            }
        };


        template <typename T>
        class Problem
        {
            bool check_input_dimensions(const std::vector<T> &x)
            {
                if (x.empty())
                {
                    common::log::warning("The solution is empty.");
                    return false;
                }
                if (x.size() != meta_data_.n_variables)
                {
                    common::log::warning("The dimension of solution is incorrect.");
                    return false;
                }
                return true;
            }

            // TODO: make the check optional
            template <typename Integer = T>
            typename std::enable_if<std::is_integral<Integer>::value, bool>::type check_input(const std::vector<T> &x)
            {
                return check_input_dimensions(x);
            }

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
                    common::log::warning("The solution contains NaN.");
                    return false;
                }
                if (common::has_inf(x))
                {
                    common::log::warning("The solution contains Inf.");
                    return false;
                }
                common::log::warning("The solution contains invalid values.");
                return false;
            }


        protected:
            MetaData meta_data_;
            Constraint<T> constraint_;
            State<T> state_;
            Solution<T> objective_;
            logger::Base *logger_{};

            [[nodiscard]]
            virtual std::vector<double> evaluate(std::vector<T> &x) = 0;


            [[nodiscard]]
            virtual std::vector<T> transform_variables(std::vector<T> x)
            {
                return x;
            }

            [[nodiscard]]
            virtual std::vector<double> transform_objectives(std::vector<double> y)
            {
                return y;
            }

        public:
            explicit Problem(MetaData meta_data, Constraint<T> constraint, Solution<T> objective) :
                meta_data_(std::move(meta_data)), constraint_(std::move(constraint)), objective_(std::move(objective))
            {
                state_ = {{
                    std::vector<T>(meta_data_.n_variables, std::numeric_limits<T>::signaling_NaN()),
                    std::vector<double>(meta_data_.n_objectives, meta_data_.initial_objective_value)
                }};
                constraint_.check_size(meta_data_.n_variables);
            }

            explicit Problem(MetaData meta_data, Constraint<T> constraint = Constraint<T>()):
                Problem(meta_data, constraint, {
                            std::vector<T>(meta_data.n_variables, std::numeric_limits<T>::signaling_NaN()),
                            std::vector<double>(meta_data.n_objectives,
                                                (meta_data.optimization_type == common::OptimizationType::minimization
                                                    ? -std::numeric_limits<double>::infinity()
                                                    : std::numeric_limits<double>::infinity()))})
            {
            }

            virtual ~Problem() = default;

            virtual void reset()
            {
                state_.reset();
                if (logger_ != nullptr)
                    logger_->track_problem(meta_data_);
            }

            [[nodiscard]]
            virtual logger::LogInfo log_info() const
            {
                const std::vector<double> positions(state_.current_best.x.begin(), state_.current_best.x.end());
                return {
                    static_cast<size_t>(state_.evaluations),
                    state_.current_internal.y.at(0),
                    state_.current_best_internal.y.at(0),
                    state_.current.y.at(0),
                    state_.current_best.y.at(0),
                    positions
                };
            }

            void attach_logger(logger::Base &logger)
            {
                logger_ = &logger;
                logger_->track_problem(meta_data_);
            }

            void detach_logger()
            {
                if (logger_ != nullptr)
                    logger_->flush();
                logger_ = nullptr;
            }

            std::vector<double> operator()(const std::vector<T> &x)
            {
                if (!check_input(x)) //TODO: make this optional
                    return std::vector<T>(meta_data_.n_objectives, std::numeric_limits<T>::signaling_NaN());

                state_.current.x = x;
                state_.current_internal.x = transform_variables(x);
                state_.current_internal.y = evaluate(state_.current_internal.x);
                state_.current.y = transform_objectives(state_.current_internal.y);
                state_.update(meta_data_, objective_);
                if (logger_ != nullptr)
                    logger_->log(log_info());
                return state_.current.y;
            }

            [[nodiscard]]
            MetaData meta_data() const
            {
                return meta_data_;
            }

            [[nodiscard]]
            Solution<T> objective() const
            {
                return objective_;
            }

            [[nodiscard]]
            State<T> state() const
            {
                return state_;
            }

            [[nodiscard]]
            Constraint<T> constraint() const
            {
                return constraint_;
            }

            friend std::ostream &operator<<(std::ostream &os, const Problem &obj)
            {
                return os
                    << "Problem(\n\t" << obj.meta_data_
                    << "\n\tconstraint: " << obj.constraint_
                    << "\n\tstate: " << obj.state_ << "\n)";
            }
        };

        template <typename T>
        using Function = std::function<std::vector<double>(std::vector<T> &)>;

        template <typename T>
        class WrappedProblem final : public Problem<T>
            //TODO: make this class registerable
        {
        protected:
            Function<T> function_;

            std::vector<double> evaluate(std::vector<T> &x) override
            {
                return function_(x);
            }

        public:
            WrappedProblem(Function<T> f, const std::string &name, const int n_variables, const int n_objectives = 1,
                           const common::OptimizationType optimization_type = common::OptimizationType::minimization,
                           Constraint<T> constraint = Constraint<T>()
                ) :
                Problem<T>(MetaData(0, name, n_variables, n_objectives, optimization_type), constraint),
                function_(f)
            {
            }
        };

        template <typename T>
        WrappedProblem<T> wrap_function(Function<T> f, const std::string &name, const int n_variables,
                                        const int n_objectives = 1,
                                        const common::OptimizationType optimization_type =
                                            common::OptimizationType::minimization,
                                        Constraint<T> constraint = Constraint<T>())
        {
            return WrappedProblem<T>{f, name, n_variables, n_objectives, optimization_type, constraint};
        }

        template <typename ProblemType>
        using ProblemFactoryType = common::RegisterWithFactory<ProblemType, int, int>;

        template <class Derived, class Parent>
        struct AutomaticProblemRegistration : common::AutomaticTypeRegistration<Derived, ProblemFactoryType<Parent>>
        {
        };

        template <class Parent>
        struct ProblemRegistry : ProblemFactoryType<Parent>
        {
        };

        using Real = Problem<double>;
        using Integer = Problem<int>;

        template<typename ProblemType>
        struct RealProblem : Real, AutomaticProblemRegistration<ProblemType, Real>
        {
            using Real::Real;
        };

        template<typename ProblemType>
        struct IntegerProblem : Integer, AutomaticProblemRegistration<ProblemType, Integer>
        {
            using Integer::Integer;
        };
    }
}
