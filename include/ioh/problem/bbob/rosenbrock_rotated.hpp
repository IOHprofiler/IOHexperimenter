#pragma once

#include "rosenbrock.hpp"


namespace ioh::problem::bbob
{
    class RosenbrockRotated final : public RosenbrockBase<RosenbrockRotated>

    {
    protected:
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            transformation::coco::transform_vars_affine_evaluate_function(x,
                transformation_state_.second_transformation_matrix, transformation_state_.transformation_base);
            return x;
        }

    public:
        RosenbrockRotated(const int instance, const int n_variables) :
            RosenbrockBase(9, instance, n_variables, "RosenbrockRotated")
        {
            const auto factor = std::max(1.0, std::sqrt(meta_data_.n_variables) / 8.0);
            for (auto i = 0; i < meta_data_.n_variables; ++i)
            {
                for (auto j = 0; j < meta_data_.n_variables; ++j)
                    transformation_state_.second_transformation_matrix[i][j] = factor * transformation_state_.second_rotation.at(i).at(j);
                transformation_state_.transformation_base[i] = 0.5;
            }

            // TODO figure out how to set xopt for this function
            // meta_data_.objective.x = std::vector<double>(meta_data_.n_variables, 1.0);
            //
            // transformation::coco::bbob2009_compute_xopt(
            //     meta_data_.objective.x, transformation_state_.seed, meta_data_.n_variables);
        }
    };
}
