#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    /// \brief Class representing the first composition function in the CEC benchmark suite.
    class CompositionFunction1 final : public CECProblem<CompositionFunction1>
    {
    protected:

        /**
         * @brief Evaluates the objective function for the given input vector.
         *
         * @param x The input vector.
         * @return The objective function value.
         */
        double evaluate(const std::vector<double>& x) override
        {
            double f = cf01(x, this->variables_shifts_, this->linear_transformations_, 1);
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

        inline static const int meta_problem_id = 1009; ///< The unique identifier for this problem.
        inline static const std::string meta_name = "CEC_CompositionFunction1"; ///< The unique name for this problem.

        /**
         * @brief Constructs a CompositionFunction1 instance.
         *
         * @param instance The instance number of the problem.
         * @param n_variables The number of variables in the problem.
         */
        CompositionFunction1(const int instance, const int n_variables) :
            CECProblem(meta_problem_id, instance, n_variables, meta_name)
        {
            this->set_optimum();
        }
    };
}
