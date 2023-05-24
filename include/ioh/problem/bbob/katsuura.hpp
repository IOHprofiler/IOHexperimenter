#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    //! Katsuura problem id 23
    template<typename P=BBOB>
    class Katsuura final : public P, BBOProblem<Katsuura>
    {
        double exponent_;
        double factor_;

    protected:

        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            auto result = 1.0;
            for (auto i = 0; i < this->meta_data_.n_variables; ++i)
            {
                double z = 0;
                for (size_t j = 1; j < 33; ++j)
                    z += fabs(this->transformation_state_.exponents[j] * x[i]
                            - floor(this->transformation_state_.exponents[j] * x[i] + 0.5))
                        / this->transformation_state_.exponents[j];

                result *= pow(1.0 + (static_cast<double>(i) + 1) * z, exponent_);
            }
            result = factor_ * (-1. + result);
            return result;
        }

        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, this->optimum_.x);
            affine(x, this->transformation_state_.second_transformation_matrix, this->transformation_state_.transformation_base);
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
            P(23, instance, n_variables, "Katsuura", sqrt(100.0)),
            exponent_(10. / pow(static_cast<double>(this->meta_data_.n_variables), 1.2)),
            factor_(10. / static_cast<double>(this->meta_data_.n_variables) / static_cast<double>(this->meta_data_.n_variables))
        {
            this->enforce_bounds(this->bounds_.weight * 1.);
            
            this->transformation_state_.exponents.resize(33);
            for (auto i = 1; i < 33; ++i)
                this->transformation_state_.exponents[i] = pow(2., static_cast<double>(i));
        }
    };
    template class Katsuura<BBOB>;
}
