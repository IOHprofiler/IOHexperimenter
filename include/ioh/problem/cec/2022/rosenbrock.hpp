#pragma once

#include "cec_problem.hpp"

/// Namespace enclosing the Implementation of Rosenbrock problem which is a part of CEC benchmark problems.
namespace ioh::problem::cec2022
{
    /// The Rosenbrock class encapsulates the Rosenbrock optimization problem, inheriting functionalities from the template class CECProblem.
    class Rosenbrock final : public CEC2022Problem<Rosenbrock>
    {
    protected:
        /// Evaluates the objective function for the Rosenbrock problem.
        /// The Rosenbrock function is computed at the given point in the search space and returned.
        /// @param x A vector representing a point in the search space.
        /// @return The value of the Rosenbrock function at point x.
        double evaluate(const std::vector<double> &x) override
        {
            return rosenbrock(x);
        }

        /// Transforms the variables using specific scale and rotate operations as defined for the Rosenbrock problem.
        /// This function performs necessary transformations on the input variables to suit the problem's specific requirements.
        /// @param x A vector of input variables.
        /// @return A vector of transformed variables which are more suited for evaluation in the Rosenbrock problem context.
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            std::vector<double> y(x.size()), z(x.size());

            transformation::variables::scale_and_rotate(x, z, y, this->variables_shifts_[0],
                                                        this->linear_transformations_[0], 2.048 / 100.0, true, true);

            return z;
        }

    public:
        /// Constructs a Rosenbrock problem instance with specified number of variables and instance identifier.
        /// @param instance An integer representing the specific problem instance number.
        /// @param n_variables An integer representing the number of variables for this problem.
        Rosenbrock(const int instance, const int n_variables) :
            CEC2022Problem(1002, instance, n_variables, "CEC2022Rosenbrock")
        {
        }
    };
}
