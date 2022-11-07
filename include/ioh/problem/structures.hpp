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
        //! Solution object
        template <typename T>
        struct Solution : common::HasRepr
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

            Solution() = default;

            /** @brief Returns true if the solution's objective has been set.
             */
            bool exists()
            {
                return y != std::numeric_limits<double>::signaling_NaN() and
                    y != std::numeric_limits<double>::infinity() and y != -std::numeric_limits<double>::infinity();
            }

            std::string repr() const override { return fmt::format("<Solution x: {} y: {}>", x, y); }
        };

        //! Multi-objective Solution object
        template <typename T>
        struct MultiObjectiveSolution : common::HasRepr
        {
            //! variables
            std::vector<T> x{};

            //! objective value
            std::vector<double > y = {};

            /**
             * @brief Construct a new Solution object
             *
             * @param x variables
             * @param y objective values
             */
            MultiObjectiveSolution(const std::vector<T> &x, const std::vector<double > y) : x(x), y(y) {}

            //! Shorthand constructor for use with unknown optimum
            MultiObjectiveSolution(const int n_variables, const int n_objectives, const common::OptimizationType optimization_type) :
                x(std::vector<T>(n_variables, std::numeric_limits<T>::signaling_NaN())),
                y{std::vector<double >(n_objectives, optimization_type == common::OptimizationType::MIN ? -std::numeric_limits<double>::infinity()
                                                                     : std::numeric_limits<double>::infinity())}
            {
            }

            MultiObjectiveSolution() = default;

            /** @brief Returns true if the solution's objective has been set.
             */
            bool exists()
            {
                return y.size() !=0 and y[0] != std::numeric_limits<double>::signaling_NaN() and
                    y[0] != std::numeric_limits<double>::infinity() and y[0] != -std::numeric_limits<double>::infinity();
            }

            bool operator==(const MultiObjectiveSolution<T> &other) const
            {
                if (y.size() == 0) return false;
                if (other.y.size() != y.size()) return false;
                for (size_t i = 0; i != y.size(); i++)
                    if (y[i] != other.y[i])
                        return false;
                return true;
            }

            std::string repr() const override { return fmt::format("<Solution x: {} y: {}>", x, y); }
        };


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

            //! the number of objectives
            int n_objectives = 1;

            /**
             * @brief Construct a new Meta Data object
             *
             * @param problem_id The id of the problem
             * @param instance The instance of the problem
             * @param name the name of the problem
             * @param n_variables the dimension of the problem
             * @param n_objectives the number of objectives
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
                    optimization_type == other.optimization_type and n_variables == other.n_variables and n_objectives == other.n_objectives;
            }

            //! comparison operator
            bool operator!=(const MetaData &other) const { return not(*this == other); }

            std::string repr() const override
            {
                return fmt::format("<MetaData: {} id: {} iid: {} dim: {}>", name, problem_id, instance, n_variables);
            }
        };


        //! Problem State`
        template <typename T>
        struct State : common::HasRepr
        {
        private:
            Solution<T> initial_solution;

        public:
            //! Current number of evaluations
            int evaluations = 0;

            //! Is optimum found?
            bool optimum_found = false;

            //! Current best x-transformed, y-raw w. constraints applied
            Solution<T> current_best_internal{};

            //! Current best x-raw, y-transformed
            Solution<T> current_best{};

            //! Current x-transformed, y-raw
            Solution<T> current_internal{};

            //! Current x-raw, y-transformed w. constraints applied
            Solution<T> current{};

            //! Current y transformed w.o. constraints applied
            double y_unconstrained;

            //! Current best y transformed w.o. constraints applied
            double y_unconstrained_best;

            //! Tracks whether the last update has caused an improvement
            bool has_improved;

            State() = default;

            /**
             * @brief Construct a new State object
             *
             * @param initial initial objective value
             */
            State(Solution<T> initial) : initial_solution(std::move(initial)) { reset(); }

            //! reset the state
            void reset()
            {
                evaluations = 0;
                current_best = initial_solution;
                current_best_internal = initial_solution;
                y_unconstrained = y_unconstrained_best = initial_solution.y;
                optimum_found = false;
                has_improved = false;
            }

            //! Update the state
            void update(const MetaData &meta_data, const Solution<T> &objective)
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
        struct MultiObjectiveState : common::HasRepr
        {
        private:
            std::vector <MultiObjectiveSolution<T> > initial_solution;

        public:
            //! Current number of evaluations
            int evaluations = 0;

            //! Is optimum found?
            bool optimum_found = false;

            //! Current x-raw, y-transformed w. constraints of the pareto front
            std::vector< MultiObjectiveSolution<T> > current_pareto_front{};
            
            //! Current x-raw, y-transformed w. constraints applied
            MultiObjectiveSolution<T> current{};

            //! current y untransformed w.o. constraints applied
            // std::vector<double > y_untransformed;

            //! Current y transformed w.o. constraints applied
            std::vector<double > y_unconstrained;

            //! Current y transformed w.o. constraints applied of the pareto front
            std::vector< std::vector<double> > y_unconstrained_pareto;

            //! Current y untransformed w.o. constraints applied of the pareto front
            // std::vector< std::vector<double> > y_untransformed_pareto;

            //! Tracks whether the last update has caused an improvement
            bool has_improved;

            MultiObjectiveState() = default;

            /**
             * @brief Construct a new State object
             *
             * @param initial initial objective value
             */
            MultiObjectiveState(MultiObjectiveSolution<T> initial)  
            {   
                if(initial_solution.size() != 0) {initial_solution.clear();}
                initial_solution.push_back(initial);
                reset(); 
            }

            //! reset the state
            void reset()
            {
                evaluations = 0;
                current_pareto_front = initial_solution;
                y_unconstrained_pareto = std::vector<std::vector<double> >(initial_solution.size());
                for (int i = 0; i != initial_solution.size(); ++i) {
                    y_unconstrained_pareto[i] = initial_solution[i].y;
                }
                optimum_found = false;
                has_improved = false;
            }

            //! Update the state
            void update(const MetaData &meta_data, const std::vector< MultiObjectiveSolution<T> > &objective)
            {
                ++evaluations;
                has_improved = false;
                std::vector<size_t> old;
                for (auto i = 0; i != current_pareto_front.size(); ++i) 
                {
                    auto dominated=true;
                    auto dominate = true;
                    for (auto j = 0; j != meta_data.n_objectives; ++j) 
                    {
                        auto d = meta_data.optimization_type(current.y[j], current_pareto_front[i].y[j]);
                        dominate &= d;
                        if (d) {
                            dominated = false;
                        }
                    }                    
                    if (!dominated) {
                        has_improved = true;
                        if (dominate) {
                            current_pareto_front.erase(current_pareto_front.begin() + i);
                            y_unconstrained_pareto.erase(y_unconstrained_pareto.begin() + i);
                            // y_transformed_pareto.erase(y_transformed_pareto.begin() + i);
                            --i;
                        }
                    }
                }
                if (has_improved) {
                    current_pareto_front.push_back(current);
                    y_unconstrained_pareto.push_back(y_unconstrained);
                    // y_transformed_pareto.push_back(y_untransformed);


                    if (current_pareto_front.size() == objective.size()) {
                        size_t i = 0;
                        while(i < objective.size()) {
                            bool found = false;
                            for (auto j = 0; j != current_pareto_front.size();  ++j) {
                                if (current_pareto_front[j] == objective[i]) {
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) 
                                break;
                            else 
                                ++i;
                        }
                        if (i == objective.size())
                            optimum_found = true;
                    }
                }
            }

            std::string repr() const override
            {
                std::vector<std::string> best;
                for (auto i = 0; i != current_pareto_front.size(); ++i) {
                    best.push_back(fmt::format("{:f}",current_pareto_front[i].y));
                }
                return fmt::format("<State evaluations: {} optimum_found: {} current_best: {}>", evaluations,
                                   optimum_found, best);
            }
        };
    } // namespace problem
} // namespace ioh
