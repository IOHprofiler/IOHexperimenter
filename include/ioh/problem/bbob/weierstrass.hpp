#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    //! Weierstrass problem id 16
    class Weierstrass final : public BBOProblem<Weierstrass>
    {
        double f0_;
        std::vector<double> ak_;
        std::vector<double> bk_;

    protected:
        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            auto result = 0.0;
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                for (size_t j = 0; j < ak_.size(); ++j)
                    result += cos(2 * IOH_PI * (x.at(i) + 0.5) * bk_.at(j)) * ak_.at(j);

            result = result / static_cast<double>(meta_data_.n_variables) - f0_;
            result = 10.0 * pow(result, 3.0);
            return result;
        }

        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, optimum_.x);
            affine(x, transformation_state_.transformation_matrix, transformation_state_.transformation_base);
            oscillate(x);
            affine(x, transformation_state_.second_transformation_matrix, transformation_state_.transformation_base);
            return x;
        }

    public:
        /**
         * @brief Construct a new Weierstrass object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        Weierstrass(const int instance, const int n_variables) :
            BBOProblem(16, instance, n_variables, "Weierstrass", 1 / sqrt(100.0)),
            f0_(0.0), ak_(12), bk_(12)
        {
            enforce_bounds(10.0 / n_variables);
            
            for (size_t i = 0; i < ak_.size(); ++i)
            {
                ak_[i] = pow(0.5, static_cast<double>(i));
                bk_[i] = pow(3., static_cast<double>(i));
                f0_ += ak_.at(i) * cos(2 * IOH_PI * bk_.at(i) * 0.5);
            }
        }
    };
}
