#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    class BentCigar final : public BBOBBase<BentCigar>

    {
    protected:
        std::vector<double> evaluate(std::vector<double> &x) override
        {
            static const auto condition = 1.0e6;
            std::vector<double> result = {x.at(0) * x.at(0)};
            for (auto i = 1; i < meta_data_.n_variables; ++i)
                result[0] += condition * x.at(i) * x.at(i);
            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            transform_vars_shift_evaluate_function(x, meta_data_.objective.x);
            transform_vars_affine_evaluate_function(x, transformation_state_.transformation_matrix, transformation_state_.transformation_base);
            transform_vars_asymmetric_evaluate_function(x, 0.5);
            transform_vars_affine_evaluate_function(x, transformation_state_.transformation_matrix, transformation_state_.transformation_base);
            return x;
        }

    public:
        BentCigar(const int instance, const int n_variables) :
            BBOBBase(12, instance, n_variables, "BentCigar")
        {
            using namespace transformation::coco;
            meta_data_.objective.x.assign(meta_data_.n_variables, 0);
            bbob2009_compute_xopt(meta_data_.objective.x,
                                  transformation_state_.seed + 1000000, meta_data_.n_variables);
    
        }
    };
}
