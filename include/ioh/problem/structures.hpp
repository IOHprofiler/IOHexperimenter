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
                y{optimization_type == common::OptimizationType::Minimization ? -std::numeric_limits<double>::infinity()
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

            //! Cast solution to double type
            [[nodiscard]] Solution<double> as_double() const { return {std::vector<double>(x.begin(), x.end()), y}; }
        };

        //! Box-Constraint object
        template <typename T>
        struct Constraint : common::HasRepr
        {

            //! lower bound
            std::vector<T> lb;

            //! Upper bound
            std::vector<T> ub;

            /**
             * @brief Construct a new Constraint object
             *
             * @param lower lower bound
             * @param upper upper bound
             */
            Constraint(const std::vector<T> &lower, const std::vector<T> &upper) : lb(lower), ub(upper) {}

            /**
             * @brief Construct a new Constraint object
             *
             * @param size size of the constraint
             * @param lower lower bound
             * @param upper upper bound
             */
            explicit Constraint(const int size = 1, const T lower = std::numeric_limits<T>::lowest(),
                                const T upper = std::numeric_limits<T>::max()) :
                Constraint(std::vector<T>(size, lower), std::vector<T>(size, upper))
            {
            }

            //! Initialization helper
            void check_size(const int s)
            {
                if (ub.size() == lb.size() && lb.size() == size_t{1})
                {
                    ub = std::vector<T>(s, ub.at(0));
                    lb = std::vector<T>(s, lb.at(0));
                }

                if ((ub.size() != static_cast<size_t>(s)) || (ub.size() != lb.size()))
                    IOH_DBG(warning, "Bound dimension is wrong");
            }

            //! Check if the constraints are violated
            bool check(const std::vector<T> &x)
            {
                for (size_t i = 0; i < x.size(); i++)
                    if (!(ub.at(i) >= x.at(i) && x.at(i) <= lb.at(i)))
                        return false;
                return true;
            }

            //! Return resize version of constraint
            Constraint<T> resize(const int s) const
            {
                return Constraint<T>(std::vector<T>(s, lb.at(0)), std::vector<T>(s, ub.at(0)));
            }

            std::string repr() const override { return fmt::format("<Constraint lb: [{}] ub: [{}]>", lb, ub); }
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

            //! Initial objective value
            double initial_objective_value{};


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
                     const common::OptimizationType optimization_type = common::OptimizationType::Minimization) :
                instance(instance),
                problem_id(problem_id), name(std::move(name)), optimization_type{optimization_type},
                n_variables(n_variables),
                initial_objective_value(optimization_type == common::OptimizationType::Minimization
                                            ? std::numeric_limits<double>::infinity()
                                            : -std::numeric_limits<double>::infinity())
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
                     const common::OptimizationType optimization_type = common::OptimizationType::Minimization) :
                MetaData(0, instance, name, n_variables, optimization_type)
            {
            }

            //! comparison operator
            bool operator==(const MetaData &other) const
            {
                return instance == other.instance and problem_id == other.problem_id and name == other.name and
                    optimization_type == other.optimization_type and n_variables == other.n_variables and
                    initial_objective_value == other.initial_objective_value;
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

            //! Current best w.o. transformations
            Solution<T> current_best_internal{};

            //! Current best w. transformations
            Solution<T> current_best{};

            //! Current w.o. transformations
            Solution<T> current_internal{};

            //! Current w. transformations
            Solution<T> current{};

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
                optimum_found = false;
            }

            //! Update the state
            void update(const MetaData &meta_data, const Solution<T> &objective)
            {
                ++evaluations;
                if (meta_data.optimization_type(current.y, current_best.y))
                {
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
    } // namespace problem
} // namespace ioh
