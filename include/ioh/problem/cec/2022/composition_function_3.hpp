#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    /// \brief Class representing the third composition function in the CEC benchmark suite.
    class CEC_CompositionFunction3 final : public CECProblem<CEC_CompositionFunction3>
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
            const int nx = static_cast<int>(x.size());
            const std::vector<double> delta = {20, 20, 30, 30, 20};
            const std::vector<double> bias = {0, 200, 300, 400, 200};

            std::vector<double> fit(5);

            expanded_schaffer_f6(x, fit[0], this->variables_shifts_[0], this->linear_transformations_[0], 1, true);
            fit[0] *= 10000.0 / 2e+7;

            std::vector<double> schwefel_z(nx);
            std::vector<double> schwefel_y(nx);
            transformation::variables::scale_and_rotate(x, schwefel_z, schwefel_y, this->variables_shifts_[1], this->linear_transformations_[1], 1000.0 / 100.0, true,
                                                        true);
            fit[1] = schwefel(schwefel_z);

            griewank(x, fit[2], this->variables_shifts_[2], this->linear_transformations_[2], 1, true);
            fit[2] *= 1000.0 / 100;

            std::vector<double> rosenbrock_z(nx);
            std::vector<double> rosenbrock_y(nx);
            transformation::variables::scale_and_rotate(x, rosenbrock_z, rosenbrock_y, this->variables_shifts_[3], this->linear_transformations_[3], 2.048 / 100.0,
                                                        true, true);
            fit[3] = rosenbrock(rosenbrock_z);

            std::vector<double> rastrigin_z(nx);
            std::vector<double> rastrigin_y(nx);
            transformation::variables::scale_and_rotate(x, rastrigin_z, rastrigin_y, this->variables_shifts_[4], this->linear_transformations_[4], 5.12 / 100.0, true,
                                                        true);
            fit[4] = rastrigin(rastrigin_z);
            fit[4] *= 10000.0 / 1e+3;

            return cf_cal(x, this->variables_shifts_, delta, bias, fit);
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
         * @brief Constructs a CEC_CompositionFunction3 instance.
         *
         * @param instance The instance number of the problem.
         * @param n_variables The number of variables in the problem.
         */
        CEC_CompositionFunction3(const int instance, const int n_variables) :
            CECProblem(1011, instance, n_variables, "CEC_CompositionFunction3")
        {
        }
    };
}
