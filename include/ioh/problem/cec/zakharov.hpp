#pragma once

#include "cec_problem.hpp"

/// Namespace enclosing the Implementation of CEC_Zakharov problem which is a part of CEC benchmark problems.
namespace ioh::problem::cec
{
    /// CEC_Zakharov class inherits from the template CECProblem with CEC_Zakharov as the template parameter.
    /// This class encapsulates the CEC_Zakharov problem for the CEC benchmark suite.
    class CEC_Zakharov final : public CECProblem<CEC_Zakharov>
    {
    protected:

        /// Evaluates the objective function for the CEC_Zakharov problem.
        /// @param x A vector representing a point in the search space.
        /// @return The objective function value at point x.
        double evaluate(const std::vector<double>& x) override
        {
            double f = zakharov(x);
            return f;
        }

        /// Transforms the variables using scale and rotate operations.
        /// It modifies the input variables to suit the problem's specific transformation requirements.
        /// @param x A vector of input variables.
        /// @return A transformed vector of variables which are suitable for the CEC_Zakharov problem.
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            std::vector<double> y(x.size()), z(x.size());

            ioh::problem::transformation::variables::scale_and_rotate(x, z, y, this->variables_shifts_[0], this->linear_transformations_[0], 1.0, 1, 1);

            return z;
        }

    public:
        /// Constructs a CEC_Zakharov problem instance.
        /// @param instance An integer representing the problem instance number.
        /// @param n_variables An integer representing the number of variables for the problem.
        CEC_Zakharov(const int instance, const int n_variables) :
            CECProblem(1001, instance, n_variables, "CEC_Zakharov")
        {
            this->set_optimum();
        }
    };
}
