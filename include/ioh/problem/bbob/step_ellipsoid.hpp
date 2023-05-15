#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{   
    //! Step ellipsiod problem id 7
    template<typename P=BBOB>
    class StepEllipsoid final : public P, BBOProblem<StepEllipsoid>
    {
    protected:
        //! compute project of x
        double compute_projection(const std::vector<double>& x)
        {
            static const auto alpha = 10.0;
            auto x0 = 0.0;
            for (auto i = 0; i < this->meta_data_.n_variables; ++i)
            {
                this->transformation_state_.transformation_base[i] = 0.0;
                for (auto j = 0; j < this->meta_data_.n_variables; ++j)
                    this->transformation_state_.transformation_base[i] += this->transformation_state_.conditions[i]
                    * this->transformation_state_.second_rotation[i][j]
                    * (x[j] - this->optimum_.x[j]);

                x0 = this->transformation_state_.transformation_base.at(0);

                this->transformation_state_.transformation_base[i] = fabs(this->transformation_state_.transformation_base[i]) > .5
                    ? floor(this->transformation_state_.transformation_base[i] + .5)
                    : floor(alpha * this->transformation_state_.transformation_base[i] + .5) / alpha;
            }
            return x0;
        }

        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            auto result = 0.0;
            auto penalty = 0.0;
            const auto x0 = compute_projection(x);

            for (auto i = 0; i < this->meta_data_.n_variables; ++i)
            {
                const auto out_of_bounds = fabs(x[i]) - 5.0;
                if (out_of_bounds > 0.0)
                    penalty += out_of_bounds * out_of_bounds;

                auto projection_sum = 0.0;
                for (auto j = 0; j < this->meta_data_.n_variables; ++j)
                    projection_sum += this->transformation_state_.first_rotation[i][j] * this->transformation_state_.transformation_base[j];

                result += pow(100., this->transformation_state_.exponents[i])
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
            P(7, instance, n_variables, "StepEllipsoid")
        {
            static const auto condition = 100.;
            for (auto i = 0; i < this->meta_data_.n_variables; ++i)
                this->transformation_state_.conditions[i] = sqrt(pow(condition / 10.,
                                                               this->transformation_state_.exponents[i]));
        }
    };
    template class StepEllipsoid<BBOB>;
}
