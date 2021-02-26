#pragma once

#include "rastrigin.hpp"

namespace ioh::problem::bbob
{
    class RastriginRotated final : public RastriginBase<RastriginRotated>
    {
    protected:
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;    
            transform_vars_shift_evaluate_function(x, meta_data_.objective.x);
            transform_vars_affine_evaluate_function(x, transformation_state_.transformation_matrix,
                transformation_state_.transformation_base);
            transform_vars_oscillate_evaluate_function(x);
            transform_vars_asymmetric_evaluate_function(x, 0.2);
            transform_vars_affine_evaluate_function(x, transformation_state_.second_transformation_matrix, 
                transformation_state_.second_transformation_base);
            return x;
        }
    public:
        RastriginRotated(const int instance, const int n_variables) :
            RastriginBase(15, instance, n_variables,  "RastriginRotated")
        {
        }
    };
}
 