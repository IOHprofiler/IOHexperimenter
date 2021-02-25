#pragma once

#include <numeric>
#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{
    class StepEllipsoid final : public BBOB<StepEllipsoid>

    {
    protected:
        double compute_projection(const std::vector<double>& x)
        {
            static const auto alpha = 10.0;
            auto x0 = 0.0;
            for (auto i = 0; i < meta_data_.n_variables; ++i)
            {
                transformation_state_.b[i] = 0.0;
                for (auto j = 0; j < meta_data_.n_variables; ++j)
                    transformation_state_.b[i] += transformation_state_.conditions.at(i)
                    * transformation_state_.rot2.at(i).at(j)
                    * (x.at(j) - meta_data_.objective.x.at(j));

                x0 = transformation_state_.b.at(0);

                transformation_state_.b[i] = fabs(transformation_state_.b.at(i)) > .5
                    ? floor(transformation_state_.b.at(i) + .5)
                    : floor(alpha * transformation_state_.b.at(i) + .5) / alpha;
            }
            return x0;
        }

        std::vector<double> evaluate(std::vector<double> &x) override
        {
            using namespace transformation::coco;
            
            std::vector<double> result{0.0};
            auto penalty = 0.0;
            const auto x0 = compute_projection(x);

            for (auto i = 0; i < meta_data_.n_variables; ++i)
            {
                const auto out_of_bounds = fabs(x.at(i)) - 5.0;
                if (out_of_bounds > 0.0)
                    penalty += out_of_bounds * out_of_bounds;

                auto projection_sum = 0.0;
                for (auto j = 0; j < meta_data_.n_variables; ++j)
                    projection_sum += transformation_state_.rot1[i][j] * transformation_state_.b[j];

                result[0] += pow(100., transformation_state_.exponents.at(i))
                    * projection_sum * projection_sum;
            }

            result[0] = 0.1 * (fabs(x0) * 1.0e-4 > result[0] ? fabs(x0) * 1.0e-4 : result[0]);
            result[0] += penalty + meta_data_.objective.y.at(0);
            return result;
        }

        std::vector<double> transform_objectives(std::vector<double> y) override
        {
            return y;
        }

    public:
        StepEllipsoid(const int instance, const int n_variables) :
            BBOB(7, instance, n_variables, "StepEllipsoid")
        {
            static const auto condition = 100.;
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                transformation_state_.conditions[i] = sqrt(pow(condition / 10.,
                                                               transformation_state_.exponents.at(i)));
        }
    };
}
