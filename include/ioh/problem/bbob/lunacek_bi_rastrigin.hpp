#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    class LunacekBiRastrigin final : public BBOProblem<LunacekBiRastrigin>

    {
    protected:
        std::vector<double> evaluate(std::vector<double> &x) override
        {
            static const auto mu0 = 2.5;
            static const auto d = 1.;
            static const auto double_n = static_cast<double>(meta_data_.n_variables);
            static const auto s = 1. - 0.5 / (sqrt(double_n + 20) - 4.1);
            static const auto mu1 = -sqrt((mu0 * mu0 - d) / s);

            auto sum1 = 0., sum2 = 0., sum3 = 0., penalty = 0.;

            std::vector<double> transformation_base(meta_data_.n_variables);
            std::vector<double> x_hat(meta_data_.n_variables);
            std::vector<double> z(meta_data_.n_variables);


            /* x_hat */
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                x_hat[i] = meta_data_.objective.x.at(i) > 0. ? 2. * x.at(i) : 2. * x.at(i) * -1;

            /* affine transformation */
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                for (auto j = 0; j < meta_data_.n_variables; ++j)
                    transformation_base[i] += transformation_state_.conditions.at(i)
                        * transformation_state_.second_rotation.at(i).at(j) * (x_hat.at(j) - mu0);

            for (auto i = 0; i < meta_data_.n_variables; ++i)
            {
                for (auto j = 0; j < meta_data_.n_variables; ++j)
                    z[i] += transformation_state_.first_rotation[i][j] * transformation_base[j];

                const auto out_of_bounds = fabs(x[i]) - 5.0;
                if (out_of_bounds > 0.0)
                    penalty += out_of_bounds * out_of_bounds;

                sum1 += (x_hat[i] - mu0) * (x_hat[i] - mu0);
                sum2 += (x_hat[i] - mu1) * (x_hat[i] - mu1);
                sum3 += cos(2 * transformation::coco::coco_pi * z[i]);
            }
            return {std::min(sum1, d * double_n + s * sum2) + 10. * (double_n - sum3) + 1e4 * penalty};
        }

    public:
        LunacekBiRastrigin(const int instance, const int n_variables) :
            BBOProblem(24, instance, n_variables, "LunacekBiRastrigin")
        {
            std::vector<double> random_normal(n_variables);
            transformation::coco::bbob2009_gauss(random_normal, n_variables, transformation_state_.seed);
            for (auto i = 0; i < n_variables; ++i)
            {
                meta_data_.objective.x[i] = random_normal.at(i) < 0.0 ? 0.5 * 2.5 * -1 : 0.5 * 2.5;
                transformation_state_.conditions[i] = pow(sqrt(100.), transformation_state_.exponents.at(i));
            }
        }
    };
}
