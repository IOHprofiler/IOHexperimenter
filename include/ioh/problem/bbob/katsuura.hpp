#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    //! Katsuura problem id 23
    class Katsuura final : public BBOProblem<Katsuura>
    {
        double exponent_;
        double factor_;

    protected:

        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            auto result = 1.0;
            for (auto i = 0; i < meta_data_.n_variables; ++i)
            {
                double z = 0;
                for (size_t j = 1; j < 33; ++j)
                    z += fabs(transformation_state_.exponents.at(j) * x.at(i)
                            - floor(transformation_state_.exponents.at(j) * x.at(i) + 0.5))
                        / transformation_state_.exponents.at(j);

                result *= pow(1.0 + (static_cast<double>(i) + 1) * z, exponent_);
            }
            result = factor_ * (-1. + result);
            return result;
        }

        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, optimum_.x);
            affine(x, transformation_state_.second_transformation_matrix, transformation_state_.transformation_base);
            return x;
        }

    public:
        /**
         * @brief Construct a new Katsuura object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        Katsuura(const int instance, const int n_variables) :
            BBOProblem(23, instance, n_variables, "Katsuura", sqrt(100.0)),
            exponent_(10. / pow(static_cast<double>(meta_data_.n_variables), 1.2)),
            factor_(10. / static_cast<double>(meta_data_.n_variables) / static_cast<double>(meta_data_.n_variables))
        {
            enforce_bounds();
            
            transformation_state_.exponents.resize(33);
            for (auto i = 1; i < 33; ++i)
                transformation_state_.exponents[i] = pow(2., static_cast<double>(i));
        }
    };
}
