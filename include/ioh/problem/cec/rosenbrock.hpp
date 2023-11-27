#pragma once

#include "cec_problem.hpp"

/// Namespace enclosing the Implementation of Rosenbrock problem which is a part of CEC benchmark problems.
namespace ioh::problem::cec
{
    /// The Rosenbrock class encapsulates the Rosenbrock optimization problem, inheriting functionalities from the template class CECProblem.
    class CEC_Rosenbrock final : public CECProblem<CEC_Rosenbrock>
    {
    protected:

        /// Evaluates the objective function for the Rosenbrock problem.
        /// The Rosenbrock function is computed at the given point in the search space and returned.
        /// @param x A vector representing a point in the search space.
        /// @return The value of the Rosenbrock function at point x.
        double evaluate(const std::vector<double>& x) override
        {
            double f = rosenbrock(x);
            return f;
        }

        /// Transforms the variables using specific scale and rotate operations as defined for the Rosenbrock problem.
        /// This function performs necessary transformations on the input variables to suit the problem's specific requirements.
        /// @param x A vector of input variables.
        /// @return A vector of transformed variables which are more suited for evaluation in the Rosenbrock problem context.
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            std::vector<double> y(x.size()), z(x.size());

            ioh::problem::transformation::variables::scale_and_rotate(x, z, y, this->variables_shifts_[0], this->linear_transformations_[0], 2.048 / 100.0, 1, 1);

            return z;
        }

    public:

        /// A static constant representing the unique identifier for the Rosenbrock problem in the CEC benchmark suite.
        inline static const int meta_problem_id = 1002;

        /// A static constant representing the name assigned to the Rosenbrock problem in the CEC benchmark suite.
        inline static const std::string meta_name = "CEC_Rosenbrock";

        /// Constructs a Rosenbrock problem instance with specified number of variables and instance identifier.
        /// @param instance An integer representing the specific problem instance number.
        /// @param n_variables An integer representing the number of variables for this problem.
        CEC_Rosenbrock(const int instance, const int n_variables) :
            CECProblem(meta_problem_id, instance, n_variables, meta_name)
        {
            this->set_optimum();
        }
    };
}
