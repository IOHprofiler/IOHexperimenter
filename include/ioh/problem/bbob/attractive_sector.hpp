#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    class AttractiveSector final : public BBOProblem<AttractiveSector>
    {
    protected:
        std::vector<double> evaluate(const std::vector<double> &x) override
        {
            std::vector<double> result{ 0.0 };
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                result[0] += x.at(i) * x.at(i) * (1. + 9999.0 * (objective_.x.at(i) * x.at(i) > 0.0));
            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            transform_vars_shift_evaluate_function(x, objective_.x);
            transform_vars_affine_evaluate_function(x, transformation_state_.second_transformation_matrix, transformation_state_.transformation_base);
            return x;
        }

        std::vector<double> transform_objectives(std::vector<double> y) override
        {
            using namespace transformation::coco;
            transform_obj_oscillate_evaluate(y);
            transform_obj_power_evaluate(y, 0.9);
            transform_obj_shift_evaluate_function(y, objective_.y.at(0));
            return y;
        }

    public:
        AttractiveSector(const int instance, const int n_variables) :
            BBOProblem(6, instance, n_variables, "AttractiveSector")
        {
        }
    };
}
