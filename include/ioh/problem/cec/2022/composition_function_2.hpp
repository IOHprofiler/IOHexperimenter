#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    /// \brief Class representing the second composition function in the CEC benchmark suite.
    class CEC_CompositionFunction2 final : public CECProblem<CEC_CompositionFunction2>
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
            const std::vector<double> delta = {20, 10, 10};
            const std::vector<double> bias = {0, 200, 100};

            std::vector<double> fit(3);

            std::vector<double> schwefel_z(nx);
            std::vector<double> schwefel_y(nx);
            transformation::variables::scale_and_rotate(x, schwefel_z, schwefel_y, this->variables_shifts_[0], this->linear_transformations_[0], 1000.0 / 100.0, true,
                                                        false);
            fit[0] = schwefel(schwefel_z);

            std::vector<double> rastrigin_z(nx);
            std::vector<double> rastrigin_y(nx);
            transformation::variables::scale_and_rotate(x, rastrigin_z, rastrigin_y, this->variables_shifts_[1], this->linear_transformations_[1], 5.12 / 100.0, true,
                                                        true);
            fit[1] = rastrigin(rastrigin_z);

            std::vector<double> hgbat_z(nx);
            std::vector<double> hgbat_y(nx);
            transformation::variables::scale_and_rotate(x, hgbat_z, hgbat_y, this->variables_shifts_[2], this->linear_transformations_[2], 5.0 / 100.0, true, true);
            fit[2] = hgbat(hgbat_z);

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
         * @brief Constructs a CEC_CompositionFunction2 instance.
         *
         * @param instance The instance number of the problem.
         * @param n_variables The number of variables in the problem.
         */
        CEC_CompositionFunction2(const int instance, const int n_variables) :
            CECProblem(1010, instance, n_variables, "CEC_CompositionFunction2")
        {
        }
    };
}
