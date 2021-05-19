#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    class Weierstrass final : public BBOProblem<Weierstrass>
    {
        double f0_;
        double penalty_factor_;
        std::vector<double> ak_;
        std::vector<double> bk_;

    protected:
        double evaluate(const std::vector<double> &x) override
        {
            using namespace transformation::coco;
            auto result = 0.0;
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                for (size_t j = 0; j < ak_.size(); ++j)
                    result += cos(2 * coco_pi * (x.at(i) + 0.5) * bk_.at(j)) * ak_.at(j);

            result = result / static_cast<double>(meta_data_.n_variables) - f0_;
            result = 10.0 * pow(result, 3.0);
            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            transform_vars_shift_evaluate_function(x, objective_.x);
            transform_vars_affine_evaluate_function(x, transformation_state_.transformation_matrix,
                                                    transformation_state_.transformation_base);
            transform_vars_oscillate_evaluate_function(x);
            transform_vars_affine_evaluate_function(x, transformation_state_.second_transformation_matrix,
                                                    transformation_state_.transformation_base);
            return x;
        }

        double transform_objectives(const double y) override
        {
            using namespace transformation::objective;
            return penalize(state_.current.x, constraint_, penalty_factor_, shift(y, objective_.y));
        }

    public:
        Weierstrass(const int instance, const int n_variables) :
            BBOProblem(16, instance, n_variables, "Weierstrass", 1 / sqrt(100.0)),
            f0_(0.0), penalty_factor_(10.0 / n_variables), ak_(12), bk_(12)
        {
            for (size_t i = 0; i < ak_.size(); ++i)
            {
                ak_[i] = pow(0.5, static_cast<double>(i));
                bk_[i] = pow(3., static_cast<double>(i));
                f0_ += ak_.at(i) * cos(2 * transformation::coco::coco_pi * bk_.at(i) * 0.5);
            }
        }
    };
}
