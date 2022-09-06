#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    //! Schefel problem id 20
    class Schwefel final : public BBOProblem<Schwefel>
    {
        std::vector<double> negative_offset_;
        std::vector<double> positive_offset_;
    protected:
        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            static const auto correction = 418.9828872724339;
            auto result = 0.0;

            auto penalty = 0.0;
            for (const auto &xi : x)
            {
                const auto out_of_bounds = fabs(xi) - 500.0;
                if (out_of_bounds > 0.0)
                    penalty += out_of_bounds * out_of_bounds;

                result += xi * sin(sqrt(fabs(xi)));
            }
            result = 0.01 * (penalty + correction - result / static_cast<double>(meta_data_.n_variables));
            return result;
        }
        
        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            transformation::variables::random_sign_flip(x, transformation_state_.seed);
            transformation::variables::scale(x, 2);
            transformation::variables::z_hat(x, optimum_.x);
            transformation::variables::subtract(x, positive_offset_);
            transformation::variables::conditioning(x, 10.0);
            transformation::variables::subtract(x, negative_offset_);
            transformation::variables::scale(x, 100);
            return x;
        }

    public:
        /**
         * @brief Construct a new Schwefel object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        Schwefel(const int instance, const int n_variables) :
            BBOProblem(20, instance, n_variables, "Schwefel"),
            negative_offset_(common::random::bbob2009::uniform(n_variables, transformation_state_.seed)),
            positive_offset_(n_variables)
        {
            for (auto i = 0; i < n_variables; ++i)
                optimum_.x[i] = (negative_offset_.at(i) < 0.5 ? -1 : 1) * 0.5 * 4.2096874637;

            for (auto i = 0; i < n_variables; ++i)
            {
                negative_offset_[i] = -2 * fabs(optimum_.x.at(i));
                positive_offset_[i] = 2 * fabs(optimum_.x.at(i));
            }
        }
    };
}
