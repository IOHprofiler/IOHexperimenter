#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{   
    //! Step ellipsiod problem id 7
    class StepEllipsoid final : public BBOProblem<StepEllipsoid>
    {
    protected:
        //! compute project of x
        double compute_projection(const std::vector<double>& x)
        {
            static const auto alpha = 10.0;
            auto x0 = 0.0;
            for (auto i = 0; i < meta_data_.n_variables; ++i)
            {
                transformation_state_.transformation_base[i] = 0.0;
                for (auto j = 0; j < meta_data_.n_variables; ++j)
                    transformation_state_.transformation_base[i] += transformation_state_.conditions.at(i)
                    * transformation_state_.second_rotation.at(i).at(j)
                    * (x.at(j) - optimum_.x.at(j));

                x0 = transformation_state_.transformation_base.at(0);

                transformation_state_.transformation_base[i] = fabs(transformation_state_.transformation_base.at(i)) > .5
                    ? floor(transformation_state_.transformation_base.at(i) + .5)
                    : floor(alpha * transformation_state_.transformation_base.at(i) + .5) / alpha;
            }
            return x0;
        }

        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            auto result = 0.0;
            auto penalty = 0.0;
            const auto x0 = compute_projection(x);

            for (auto i = 0; i < meta_data_.n_variables; ++i)
            {
                const auto out_of_bounds = fabs(x.at(i)) - 5.0;
                if (out_of_bounds > 0.0)
                    penalty += out_of_bounds * out_of_bounds;

                auto projection_sum = 0.0;
                for (auto j = 0; j < meta_data_.n_variables; ++j)
                    projection_sum += transformation_state_.first_rotation[i][j] * transformation_state_.transformation_base[j];

                result += pow(100., transformation_state_.exponents.at(i))
                    * projection_sum * projection_sum;
            }

            result = 0.1 * (fabs(x0) * 1.0e-4 > result ? fabs(x0) * 1.0e-4 : result);
            return result + penalty;
        }

    public:
        /**
         * @brief Construct a new Step Ellipsoid object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        StepEllipsoid(const int instance, const int n_variables) :
            BBOProblem(7, instance, n_variables, "StepEllipsoid")
        {
            static const auto condition = 100.;
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                transformation_state_.conditions[i] = sqrt(pow(condition / 10.,
                                                               transformation_state_.exponents.at(i)));
        }
    };
}
