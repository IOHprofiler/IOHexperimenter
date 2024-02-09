#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    /// \brief Class representing the fourth composition function in the CEC benchmark suite.
    class CEC_CompositionFunction4 final : public CECProblem<CEC_CompositionFunction4>
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
            double f = cf07(x, this->variables_shifts_, this->linear_transformations_, 1);
            return f;
        }

        /**
         * @brief Transforms the input variables based on the current transformation data.
         *
         * @param x The original input variables.
         * @return The transformed input variables.
         */
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            return x;
        }

    public:
        /**
         * @brief Constructs a CEC_CompositionFunction4 instance.
         *
         * @param instance The instance number of the problem.
         * @param n_variables The number of variables in the problem.
         */
        CEC_CompositionFunction4(const int instance, const int n_variables) :
            CECProblem(1012, instance, n_variables, "CEC_CompositionFunction4")
        {
        }
    };
}
