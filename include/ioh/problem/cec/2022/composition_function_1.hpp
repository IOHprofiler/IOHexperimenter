#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec2022
{
    /// \brief Class representing the first composition function in the CEC benchmark suite.
    class CompositionFunction1 final : public CEC2022Problem<CompositionFunction1>
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
            const std::vector<double> deltas = {10, 20, 30, 40, 50};
            const std::vector<double> biases = {0, 200, 300, 100, 400};
            std::vector<double> fit(5);

            const int nx = static_cast<int>(x.size());

            std::vector<double> rosenbrock_z(nx);
            std::vector<double> rosenbrock_y(nx);
            transformation::variables::scale_and_rotate(x, rosenbrock_z, rosenbrock_y, this->variables_shifts_[0], this->linear_transformations_[0], 2.048 / 100.0,
                                                        true, true);
            fit[0] = rosenbrock(rosenbrock_z);
            fit[0] = 10000 * fit[0] / 1e+4;

            std::vector<double> ellipse_z(nx);
            std::vector<double> ellipsis_y(nx);
            transformation::variables::scale_and_rotate(x, ellipse_z, ellipsis_y, this->variables_shifts_[1], this->linear_transformations_[1], 1.0, true, false);
            fit[1] = ellipse(ellipse_z);
            fit[1] = 10000 * fit[1] / 1e+10;

            std::vector<double> bent_cigar_z(nx);
            std::vector<double> bent_cigar_y(nx);
            transformation::variables::scale_and_rotate(x, bent_cigar_z, bent_cigar_y, this->variables_shifts_[2], this->linear_transformations_[2], 1.0, true, true);
            fit[2] = bent_cigar(bent_cigar_z);
            fit[2] = 10000 * fit[2] / 1e+30;

            std::vector<double> discus_z(nx);
            std::vector<double> discus_y(nx);
            transformation::variables::scale_and_rotate(x, discus_z, discus_y, this->variables_shifts_[3], this->linear_transformations_[3], 1.0, true, true);
            fit[3] = discus(discus_z);
            fit[3] = 10000 * fit[3] / 1e+10;

            std::vector<double> ellipse_2_z(nx);
            std::vector<double> ellipse_2_y(nx);
            transformation::variables::scale_and_rotate(x, ellipse_2_z, ellipse_2_y, this->variables_shifts_[4], this->linear_transformations_[4], 1.0, true, false);
            fit[4] = ellipse(ellipse_2_z);
            fit[4] = 10000 * fit[4] / 1e+10;

            return cf_cal(x, this->variables_shifts_, deltas, biases, fit);
        }

    public:
        /**
         * @brief Constructs a CEC_CompositionFunction1 instance.
         *
         * @param instance The instance number of the problem.
         * @param n_variables The number of variables in the problem.
         */
        CompositionFunction1(const int instance, const int n_variables) :
            CEC2022Problem(1009, instance, n_variables, "CEC2022CompositionFunction1")
        {
        }
    };
}
