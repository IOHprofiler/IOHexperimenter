#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    //! GriewankRosenbrock problem id 19
    template <typename P = BBOB>
    class GriewankRosenbrock final : public P, BBOProblem<GriewankRosenbrock>
    {
        std::vector<double> x_shift_;

    protected:
        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            auto result = 0.0;
            for (auto i = 0; i < this->meta_data_.n_variables - 1; ++i)
            {
                const auto c1 = 100.0 * pow(pow(x[i], 2.0) - x.at(static_cast<size_t>(i) + 1), 2.0);
                const auto c2 = pow(1.0 - x[i], 2.0);
                const auto z = c1 + c2;
                result += z / 4000. - cos(z);
            }
            return 10. + 10. * result / (static_cast<double>(this->meta_data_.n_variables) - 1.);
        }

        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            affine(x, this->transformation_state_.second_rotation, this->transformation_state_.transformation_base);
            subtract(x, x_shift_);
            return x;
        }

    public:
        /**
         * @brief Construct a new Griewank Rosen Brock object
         *
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        GriewankRosenbrock(const int instance, const int n_variables) :
            P(19, instance, n_variables, "GriewankRosenbrock"), x_shift_(n_variables, -0.5)
        {
            const auto factor = std::max(1., sqrt(n_variables) / 8.);

            for (auto i = 0; i < n_variables; ++i)
            {
                auto sum = 0.0;
                for (auto j = 0; j < n_variables; ++j)
                {
                    this->transformation_state_.second_rotation[i][j] *= factor;
                    sum += this->transformation_state_.second_rotation[j][i];
                }
                this->optimum_.x[i] = sum / (2. * factor);
            }
        }
    };
    template class GriewankRosenbrock<BBOB>;
} // namespace ioh::problem::bbob
