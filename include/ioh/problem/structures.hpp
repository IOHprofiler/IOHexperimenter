#pragma once

#include <limits>
#include <utility>
#include <vector>

#include "ioh/common/log.hpp"
#include "ioh/common/optimization_type.hpp"
#include "ioh/common/repr.hpp"
#include "ioh/common/pareto.hpp"

namespace ioh
{
    namespace problem
    {
        //! Typedef for single objective problems
        using SingleObjective = double;

        //! Typedef for mutliobjective problems
        using MultiObjective = std::vector<double>;

        //! struct of problem meta data
        struct MetaData : common::HasRepr
        {
            // Most of fields here are `int` and not `unsigned long` (or `size_t`) because of interoperability with some
            // problem suites.

            //! Instance id
            int instance{};

            //! problem id
            int problem_id{};

            //! problem name
            std::string name;

            //! optimization type
            common::FOptimizationType optimization_type;

            //! problem dimension
            int n_variables{};

            //! problem objectives
            int n_objectives{};

            //! Final target
            double final_target;

            /**
             * @brief Construct a new Meta Data object
             *
             * @param problem_id The id of the problem
             * @param instance The instance of the problem
             * @param name the name of the problem
             * @param n_variables the dimension of the problem
             * @param optimization_type optimization type
             * @param final_target the final target to be reached for the function
             */
            MetaData(const int problem_id, const int instance, std::string name,
                     const int n_variables, const int n_objectives,
                     const common::OptimizationType optimization_type = common::OptimizationType::MIN,
                     const double final_target = 1e-8) :
                instance(instance),
                problem_id(problem_id), 
                name(std::move(name)), 
                optimization_type{optimization_type},
                n_variables(n_variables), 
                n_objectives(n_objectives),
                final_target(final_target)
            {
            }

            /**
             * @brief Construct a new Meta Data object
             *
             * @param instance The instance of the problem
             * @param name the name of the problem
             * @param n_variables the dimension of the problem
             * @param optimization_type optimization type
             * @param final_target the final target to be reached for the function
             */
            MetaData(const int instance, const std::string &name, 
                     const int n_variables, const int n_objectives,
                     const common::OptimizationType optimization_type = common::OptimizationType::MIN,
                     const double final_target = 1e-8) :
                MetaData(0, instance, name, n_variables, n_objectives, optimization_type, final_target)
            {
            }

            /**
             * @brief Construct a new Meta Data object
             *
             * @param problem_id The id of the problem
             * @param instance The instance of the problem
             * @param name the name of the problem
             * @param n_variables the dimension of the problem
             * @param optimization_type optimization type
             * @param final_target the final target to be reached for the function
             */
             MetaData(const int problem_id, const int instance, std::string name,
                const int n_variables,
                const common::OptimizationType optimization_type = common::OptimizationType::MIN,
                const double final_target = 1e-8) :
                instance(instance),
                problem_id(problem_id), 
                name(std::move(name)), 
                optimization_type{optimization_type},
                n_variables(n_variables), 
                n_objectives(1),
                final_target(final_target)
            {
            }

            /**
            * @brief Construct a new Meta Data object
            *
            * @param instance The instance of the problem
            * @param name the name of the problem
            * @param n_variables the dimension of the problem
            * @param optimization_type optimization type
            * @param final_target the final target to be reached for the function
            */
            MetaData(const int instance, const std::string &name, 
                const int n_variables, 
                const common::OptimizationType optimization_type = common::OptimizationType::MIN,
                const double final_target = 1e-8) :
                MetaData(0, instance, name, n_variables, optimization_type, final_target)
            {
            }


            //! comparison operator
            bool operator==(const MetaData &other) const
            {
                return instance == other.instance and problem_id == other.problem_id and name == other.name and
                    optimization_type == other.optimization_type and n_variables == other.n_variables and n_objectives == other.n_objectives;
            }

            //! comparison operator
            bool operator!=(const MetaData &other) const { return not(*this == other); }

            [[nodiscard]] std::string repr() const override
            {
                return fmt::format("<MetaData: {} id: {} iid: {} dim: {} obj: {}>", name, problem_id, instance, n_variables, n_objectives);
            }
        };


        template <typename T, typename R>
        struct Solution;

        //! Solution object
        template <typename T>
        struct Solution<T, SingleObjective> : common::HasRepr
        {
            //! variables
            std::vector<T> x{};

            //! objective value
            double y = std::numeric_limits<double>::signaling_NaN();

            /**
             * @brief Construct a new Solution object
             *
             * @param x variables
             * @param y objective value
             */
            Solution(const std::vector<T> &x, const double y) : x(x), y(y) {}

            //! Shorthand constructor for use with unknown optimum
            Solution(const int n_variables, const common::OptimizationType optimization_type) :
                x(std::vector<T>(n_variables, std::numeric_limits<T>::signaling_NaN())),
                y{optimization_type == common::OptimizationType::MIN ? -std::numeric_limits<double>::infinity()
                                                                     : std::numeric_limits<double>::infinity()}
            {
            }
            Solution(const int n_variables, const int n_objectives, const common::OptimizationType optimization_type) :
                x(std::vector<T>(n_variables, std::numeric_limits<T>::signaling_NaN())),
                y{optimization_type == common::OptimizationType::MIN ? -std::numeric_limits<double>::infinity()
                                                                     : std::numeric_limits<double>::infinity()}
            {
            }


            //! Default Constructible
            Solution() = default;

            /** @brief Returns true if the solution's objective has been set.
             */
            [[nodiscard]] bool exists() const
            {
                return y != std::numeric_limits<double>::signaling_NaN() and
                    y != std::numeric_limits<double>::infinity() and y != -std::numeric_limits<double>::infinity();
            }

            [[nodiscard]] std::string repr() const override { return fmt::format("<Solution x: {} y: {}>", x, y); }
        };

        //! Solution object
        template <typename T>
        struct Solution<T, MultiObjective> : common::HasRepr
        {
            //! variables
            std::vector<T> x{};

            //! objective value
            std::vector<double> y = {};

            /**
             * @brief Construct a new Solution object
             *
             * @param x variables
             * @param y objective values
             */
            Solution(const std::vector<T> &x, const std::vector<double> &y) : x(x), y(y) {}
            
            //! Shorthand constructor for use with unknown optimum
            Solution(const int n_variables, const int n_objectives, const common::OptimizationType optimization_type) :
                x(std::vector<T>(n_variables, std::numeric_limits<T>::signaling_NaN())),
                y(std::vector<double>(n_objectives, optimization_type == common::OptimizationType::MIN 
                    ? -std::numeric_limits<double>::infinity() 
                    : std::numeric_limits<double>::infinity()))
            {
            }

            //! Default constructible
            Solution() = default;

            //! Returns true if the solution's objective has been set.
            [[nodiscard]] bool exists() const
            {
                return y.size() != 0 and y[0] != std::numeric_limits<double>::signaling_NaN() and
                    y[0] != std::numeric_limits<double>::infinity() and
                    y[0] != -std::numeric_limits<double>::infinity();
            }

            //! Equality operator
            bool operator==(const Solution<T, MultiObjective> &other) const
            {
                if (y.size() == 0)
                    return false;
                if (other.y.size() != y.size())
                    return false;
                for (size_t i = 0; i != y.size(); ++i)
                    if (y[i] != other.y[i])
                        return false;
                return true;
            }
            //! String representation
            [[nodiscard]] std::string repr() const override
            {
                return fmt::format("<Solution x: [{}] y: [{}]>", fmt::join(x, ","), fmt::join(y, ","));
            }
        };


        template <typename T, typename R>
        struct State;

        //! Problem State`
        template <typename T>
        struct State<T, double> : common::HasRepr
        {
        private:
            Solution<T, double> initial_solution_;

        public:
            //! Current number of evaluations
            int evaluations = 0;

            //! Is optimum found?
            bool optimum_found = false;

            //! final target found?
            bool final_target_found = false;

            //! Current best x-transformed, y-raw w. constraints applied
            Solution<T, double> current_best_internal{};

            //! Current best x-raw, y-transformed
            Solution<T, double> current_best{};

            //! Current x-transformed, y-raw
            Solution<T, double> current_internal{};

            //! Current x-raw, y-transformed w. constraints applied
            Solution<T, double> current{};

            //! Current y transformed w.o. constraints applied
            double y_unconstrained;

            //! Current y transformed w.o. constraints applied
            double y_unconstrained_best;

            //! Tracks whether the last update has caused an improvement
            bool has_improved;

            State() = default;

            /**
             * @brief Construct a new State object
             *
             * @param initial initial objective value
             */
            State(Solution<T, double> initial) : initial_solution_(std::move(initial)) { reset(); }

            //! reset the state
            void reset()
            {
                evaluations = 0;
                current_best = initial_solution_;
                current_best_internal = initial_solution_;
                y_unconstrained = y_unconstrained_best = initial_solution_.y;
                optimum_found = false;
                has_improved = false;
                final_target_found = false;
            }

            void invert() { initial_solution_.y = -initial_solution_.y; }

            //! Update the state
            void update(const MetaData &meta_data, const Solution<T, double> &objective)
            {
                ++evaluations;

                const bool has_internal_improved = meta_data.optimization_type(current_internal.y, current_best_internal.y);
                has_improved = meta_data.optimization_type(current.y, current_best.y);

                // TODO: Clean this up; dont use the meta data like this
                if (meta_data.problem_id >= 10'000 && meta_data.problem_id <= 10'004 && has_internal_improved)
                {
                    current_best_internal = current_internal;
                }

                // This calls the operator() of a class. See: include/ioh/common/optimization_type.hpp:64
                if (has_improved)
                {
                    y_unconstrained_best = y_unconstrained;
                    current_best_internal = current_internal;
                    current_best = current;

                    if (std::abs(objective.y - current.y) < std::numeric_limits<double>::epsilon())
                        optimum_found = true;

                    if (std::abs(objective.y - current.y) < meta_data.final_target)
                        final_target_found = true;
                }
            }

            [[nodiscard]] std::string repr() const override
            {
                return fmt::format("<State evaluations: {} final_target_found: {} current_best: {}>", evaluations,
                                   final_target_found, current_best);
            }
        };


        //! Problem State`
        template <typename T>
        struct State<T, MultiObjective> : common::HasRepr
        {
        private:
            // Solution<T, double> initial_solution;
            std::vector<Solution<T, MultiObjective>> initial_pareto_front_ {};

            int pareto_order(
                const Solution<T, MultiObjective> &a, 
                const std::vector<Solution<T, MultiObjective>> &current_pareto_front,
                common::FOptimizationType optimization_type
            )
            {
                bool better = true;
                bool worse = true;
                for (const auto &b : current_pareto_front)
                {
                    if(!common::pareto::dominates(a.y, b.y, optimization_type))
                    {
                        better = false;
                    }
                    if (!common::pareto::dominates(b.y, a.y, optimization_type))
                    {
                        worse = false;
                    }
                }
                if(better && !worse)
                {
                    return 1; // a is better
                }
                else if(!better && worse)
                {
                    return -1; // a is worse
                }
                else
                {
                    return 0; // a is equal
                }
            }
            

        public:
            //! Current number of evaluations
            int evaluations = 0;

            //! Is optimum found?
            bool optimum_found = false;
            
            bool has_improved = false;
            bool final_target_found = false;


            //! Current x-raw, y-transformed w. constraints applied
            Solution<T, MultiObjective> current{};

            std::vector<Solution<T, MultiObjective>> pareto_front{};
           
            State() = default;

            State(Solution<T, MultiObjective> initial){ 
                initial_pareto_front_.push_back(initial);
                reset(); 
            }

            [[nodiscard]] std::string repr() const override
            {
                return fmt::format("<State evaluations: {} optimum_found: {} pareto_front: [{}]>", 
                                   evaluations, 
                                   optimum_found, 
                                   fmt::join(pareto_front, ", "));
            }

            //! Update the state
            void update(const MetaData &meta_data, const Solution<T, MultiObjective> &objective) {
                ++evaluations;

                // //use pareto front here
                const int pareto_order = this->pareto_order(current, pareto_front, meta_data.optimization_type);
                if (pareto_order == 1)
                {
                    pareto_front = {current};
                    has_improved = true;
                }
                else if (pareto_order == 0)
                {
                    // Remove all solutions in the pareto front that are dominated by the current solution
                    pareto_front.erase(
                        std::remove_if(
                            pareto_front.begin(), pareto_front.end(),
                            [this, &meta_data](const Solution<T, MultiObjective> &solution) {
                                return common::pareto::dominates(this->current.y, solution.y, meta_data.optimization_type);
                            }),
                        pareto_front.end());

                    // Add the current solution to the pareto front
                    pareto_front.push_back(current);
                    
                    has_improved = true;
                }
                


                // // This calls the operator() of a class. See: include/ioh/common/optimization_type.hpp:64
                if (has_improved)
                {
                    if (common::pareto::vector_difference(objective.y, current.y) < std::numeric_limits<double>::epsilon())
                        optimum_found = true;

                    if (common::pareto::vector_difference(objective.y, current.y) < meta_data.final_target)
                        final_target_found = true;
                }
            }
            
            void invert() { 

                for (auto &solution : initial_pareto_front_) {
                    for (auto &value : solution.y) {
                        value = -value;
                    }
                }
            }

            //! Reset the state
            void reset()
            {
                evaluations = 0;
                pareto_front = initial_pareto_front_;
                optimum_found = false;
                has_improved = false;
                final_target_found = false;
            }
        };


    } // namespace problem
} // namespace ioh
