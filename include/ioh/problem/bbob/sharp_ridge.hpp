#pragma once

#include "bbob_problem.hpp"

//! BBOB namespace
namespace ioh::problem::bbob
{
    //! Sharp ridge function problem id 13
    template<typename P=BBOB>
    class SharpRidge final : public P, BBOProblem<SharpRidge>
    {
        int n_linear_dimensions_;
    protected:
        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            static const auto alpha = 100.0;

            auto result = 0.0;
            for (auto i = n_linear_dimensions_; i < this->meta_data_.n_variables; ++i)
                result += x[i] * x[i];

            result = alpha * sqrt(result / n_linear_dimensions_);
            for (auto i = 0; i < n_linear_dimensions_; ++i)
                result += x[i] * x[i] / n_linear_dimensions_;

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
         * @brief Construct a new Sharp Ridge object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem 
         */
        SharpRidge(const int instance, const int n_variables) :
            P(13, instance, n_variables, "SharpRidge"),
        n_linear_dimensions_(static_cast<int>(
            ceil(this->meta_data_.n_variables <= 40 ? 1 : this->meta_data_.n_variables / 40.0)))
        {
        }
    };

    template class SharpRidge<BBOB>;
}
