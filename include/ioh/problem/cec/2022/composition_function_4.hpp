#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec2022
{
    /// \brief Class representing the fourth composition function in the CEC benchmark suite.
    class CompositionFunction4 final : public CEC2022Problem<CompositionFunction4>
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
            const std::vector<double> deltas = {10, 20, 30, 40, 50, 60};
            const std::vector<double> biases = {0, 300, 500, 100, 400, 200};
            const std::vector<double> sh_rates = {5.0 / 100.0, 5.12 / 100.0, 10.0, 1.0, 1.0, 1.0};
            const std::vector<bool> s_flags = {true, true, true, true, true, true};
            const std::vector<bool> r_flags = {true, true, true, true, true, true};
            const std::vector<double> f_scale = {1000, 1e+3, 4e+3, 1e+30, 1e+10, 2e+7};
            const std::vector<RealFunction> functions = {hgbat, rastrigin, schwefel, bent_cigar, ellipse, expanded_schaffer};

            return calculate_composite(x, this->variables_shifts_, this->linear_transformations_, deltas, biases,
                                       sh_rates, s_flags, r_flags, f_scale, functions);

        }
    public:
        /**
         * @brief Constructs a CEC_CompositionFunction4 instance.
         *
         * @param instance The instance number of the problem.
         * @param n_variables The number of variables in the problem.
         */
        CompositionFunction4(const int instance, const int n_variables) :
            CEC2022Problem(1012, instance, n_variables, "CEC2022CompositionFunction4")
        {
        }
    };
}
