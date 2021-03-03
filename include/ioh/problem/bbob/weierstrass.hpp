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
        std::vector<double> raw_x_;

    protected:
        std::vector<double> evaluate(const std::vector<double> &x) override
        {
            using namespace transformation::coco;
            std::vector<double> result = {0.0};
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                for (size_t j = 0; j < ak_.size(); ++j)
                    result[0] += cos(2 * coco_pi * (x.at(i) + 0.5) * bk_.at(j)) * ak_.at(j);
            
            result[0] = result.at(0) / static_cast<double>(meta_data_.n_variables) - f0_;
            result[0] = 10.0 * pow(result.at(0), 3.0);
            return result;
    
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            raw_x_ = x;
            transform_vars_shift_evaluate_function(x, objective_.x);
            transform_vars_affine_evaluate_function(x, transformation_state_.transformation_matrix,
                                                    transformation_state_.transformation_base);
            transform_vars_oscillate_evaluate_function(x);
            transform_vars_affine_evaluate_function(x, transformation_state_.second_transformation_matrix,
                                                    transformation_state_.transformation_base);
            return x;
        }

        std::vector<double> transform_objectives(std::vector<double> y) override
        {
            using namespace transformation::coco;
            transform_obj_shift_evaluate_function(y, objective_.y.at(0));
            transform_obj_penalize_evaluate(raw_x_, constraint_.lb.at(0),
                                            constraint_.ub.at(0), penalty_factor_, y);
            return y;
        }

    public:
        Weierstrass(const int instance, const int n_variables) :
            BBOProblem(16, instance, n_variables, "Weierstrass", 1 / sqrt(100.0)),
            f0_(0.0), ak_(12), bk_(12), raw_x_(n_variables), penalty_factor_(10.0/ n_variables)
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
