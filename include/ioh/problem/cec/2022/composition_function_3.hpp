#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec2022
{
    /// \brief Class representing the third composition function in the CEC benchmark suite.
    class CompositionFunction3 final : public CEC2022Problem<CompositionFunction3>
    {
    protected:
        /**
         * @brief Evaluates the objective function for the given input vector.
         *
         * @param x The input vector.
         * @return The objective function value.
         */
        double evaluate(const std::vector<double> &x) override
        {
            const std::vector<double> deltas = {20, 20, 30, 30, 20};
            const std::vector<double> biases = {0, 200, 300, 400, 200};
            const std::vector<double> sh_rates = {1, 10.0, 6.0, 2.048 / 100.0, 5.12 / 100.0};
            const std::vector<bool> s_flags = {true, true, true, true, true};
            const std::vector<bool> r_flags = {true, true, true, true, true};
            const std::vector<double> f_scale = {2e+7, 1e+4, 1e+3, 1e+4, 1e+3};
            const std::vector<RealFunction> functions = {expanded_schaffer, schwefel, griewank, rosenbrock,
                                                         rastrigin};
            return calculate_composite(x, this->variables_shifts_, this->linear_transformations_, deltas, biases,
                                       sh_rates, s_flags, r_flags, f_scale, functions);
        }

    public:
        /**
         * @brief Constructs a CEC_CompositionFunction3 instance.
         *
         * @param instance The instance number of the problem.
         * @param n_variables The number of variables in the problem.
         */
        CompositionFunction3(const int instance, const int n_variables) :
            CEC2022Problem(1011, instance, n_variables, "CEC2022CompositionFunction3")
        {
        }
    };
}
