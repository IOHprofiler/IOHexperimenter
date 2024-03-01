#pragma once

#include <limits>
#include <utility>
#include <vector>

#include "ioh/common/log.hpp"
#include "ioh/common/optimization_type.hpp"
#include "ioh/common/repr.hpp"

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

            /**
             * @brief Construct a new Meta Data object
             *
             * @param problem_id The id of the problem
             * @param instance The instance of the problem
             * @param name the name of the problem
             * @param n_variables the dimension of the problem
             * @param optimization_type optimization type
             */
            MetaData(const int problem_id, const int instance, std::string name, const int n_variables,
                     const common::OptimizationType optimization_type = common::OptimizationType::MIN) :
                instance(instance),
                problem_id(problem_id), name(std::move(name)), optimization_type{optimization_type},
                n_variables(n_variables)
            {
            }

            /**
             * @brief Construct a new Meta Data object
             *
             * @param instance The instance of the problem
             * @param name the name of the problem
             * @param n_variables the dimension of the problem
             * @param optimization_type optimization type
             */
            MetaData(const int instance, const std::string &name, const int n_variables,
                     const common::OptimizationType optimization_type = common::OptimizationType::MIN) :
                MetaData(0, instance, name, n_variables, optimization_type)
            {
            }

            //! comparison operator
            bool operator==(const MetaData &other) const
            {
                return instance == other.instance and problem_id == other.problem_id and name == other.name and
                    optimization_type == other.optimization_type and n_variables == other.n_variables;
            }

            //! comparison operator
            bool operator!=(const MetaData &other) const { return not(*this == other); }

            std::string repr() const override
            {
                return fmt::format("<MetaData: {} id: {} iid: {} dim: {}>", name, problem_id, instance, n_variables);
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

            //! Default Constructible
            Solution() = default;

            /** @brief Returns true if the solution's objective has been set.
             */
            bool exists() const
            {
                return y != std::numeric_limits<double>::signaling_NaN() and
                    y != std::numeric_limits<double>::infinity() and y != -std::numeric_limits<double>::infinity();
            }

            std::string repr() const override { return fmt::format("<Solution x: {} y: {}>", x, y); }
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
            Solution(const std::vector<T> &x, const std::vector<double>& y) : x(x), y(y) {}

            //! Default constructible
            Solution() = default;

            //! Returns true if the solution's objective has been set.
            bool exists() const
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
            std::string repr() const override
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
            Solution<T, double> initial_solution;

        public:
            //! Current number of evaluations
            int evaluations = 0;

            //! Is optimum found?
            bool optimum_found = false;

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

            //! The value of additive noise
            double y_noise;

            //! The y value without additive noise
            double y_noiseless;

            State() = default;

            /**
             * @brief Construct a new State object
             *
             * @param initial initial objective value
             */
            State(Solution<T, double> initial) : initial_solution(std::move(initial)) { reset(); }

            //! reset the state
            void reset()
            {
                evaluations = 0;
                current_best = initial_solution;
                current_best_internal = initial_solution;
                y_unconstrained = y_unconstrained_best = initial_solution.y;
                optimum_found = false;
                has_improved = false;
                y_noise = 0.;
                y_noiseless = 0.;
            }

            void invert(){
                initial_solution.y = -initial_solution.y;
            }

            //! Update the state
            void update(const MetaData &meta_data, const Solution<T, double> &objective)
            {
                ++evaluations;
                has_improved = meta_data.optimization_type(current.y, current_best.y);
                if (has_improved)
                {
                    y_unconstrained_best = y_unconstrained;
                    current_best_internal = current_internal;
                    current_best = current;

                    if (objective.y == current.y)
                        optimum_found = true;
                }
            }

            std::string repr() const override
            {
                return fmt::format("<State evaluations: {} optimum_found: {} current_best: {}>", evaluations,
                                   optimum_found, current_best);
            }
        };


         //! Problem State`
        template <typename T>
        struct State<T, MultiObjective> : common::HasRepr
        {
        private:
            //Solution<T, double> initial_solution;

        public:
            //! Current number of evaluations
            int evaluations = 0;

            //! Is optimum found?
            bool optimum_found = false;
            
            std::string repr() const override
            {
                return fmt::format("<State evaluations: {} optimum_found: {}>", evaluations,
                                   optimum_found);
            }

            //! Update the state
            void update(const MetaData &meta_data, const Solution<T, MultiObjective> &objective) { }
            
            //! Reset the state
            void reset() {
                evaluations = 0;
                optimum_found = false;
            }
        };




    } // namespace problem
} // namespace ioh
