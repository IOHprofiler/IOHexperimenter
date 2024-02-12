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
            const int nx = static_cast<int>(x.size());
            std::vector<double> fit(6);
            const std::vector<double> delta = {10, 20, 30, 40, 50, 60};
            const std::vector<double> bias = {0, 300, 500, 100, 400, 200};

            std::vector<double> hgbat_z(nx);
            std::vector<double> hgbat_y(nx);
            transformation::variables::scale_and_rotate(x, hgbat_z, hgbat_y, this->variables_shifts_[0], this->linear_transformations_[0], 5.0 / 100.0, true, true);
            fit[0] = hgbat(hgbat_z);
            fit[0] *= 10000.0 / 1000;

            std::vector<double> rastrigin_z(nx);
            std::vector<double> rastrigin_y(nx);
            transformation::variables::scale_and_rotate(x, rastrigin_z, rastrigin_y, this->variables_shifts_[1], this->linear_transformations_[1], 5.12 / 100.0, true,
                                                        true);
            fit[1] = rastrigin(rastrigin_z);
            fit[1] *= 10000.0 / 1e+3;

            std::vector<double> schwefel_z(nx);
            std::vector<double> schwefel_y(nx);
            transformation::variables::scale_and_rotate(x, schwefel_z, schwefel_y, this->variables_shifts_[2], this->linear_transformations_[2], 1000.0 / 100.0, true,
                                                        true);
            fit[2] = schwefel(schwefel_z);
            fit[2] *= 10000.0 / 4e+3;

            // bent_cigar(x, fit[3], Os[3], Mr[3], 1, true);
            // fit[3] *= 10000 / 1e+30;

            std::vector<double> ellipse_z(nx);
            std::vector<double> ellipse_y(nx);
            transformation::variables::scale_and_rotate(x, ellipse_z, ellipse_y, this->variables_shifts_[4], this->linear_transformations_[4], 1.0, true, true);
            fit[4] = ellipse(ellipse_z);
            fit[4] *= 10000 / 1e+10;

            expanded_schaffer_f6(x, fit[5], this->variables_shifts_[5], this->linear_transformations_[5], 1, true);
            fit[5] *= 10000 / 2e+7;
            return cf_cal(x, this->variables_shifts_, delta, bias, fit);
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
