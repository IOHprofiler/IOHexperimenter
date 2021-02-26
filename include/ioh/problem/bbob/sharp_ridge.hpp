#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{
    class SharpRidge final : public BBOB<SharpRidge>

    {
    protected:
        std::vector<double> evaluate(std::vector<double> &x) override
        {
            static const auto alpha = 100.0;
            static const auto n_linear_dimensions = static_cast<int>(
                ceil(meta_data_.n_variables <= 40 ? 1 : meta_data_.n_variables / 40.0));

            std::vector<double> result{0.0};
            for (auto i = n_linear_dimensions; i < meta_data_.n_variables; ++i)
                result[0] += x.at(i) * x.at(i);

            result[0] = alpha * sqrt(result.at(0) / n_linear_dimensions);
            for (auto i = 0; i < n_linear_dimensions; ++i)
                result[0] += x.at(i) * x.at(i) / n_linear_dimensions;

            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            transform_vars_shift_evaluate_function(x, meta_data_.objective.x);
            transform_vars_affine_evaluate_function(x, transformation_state_.second_transformation_matrix, transformation_state_.second_transformation_base);
            return x;
        }

    public:
        SharpRidge(const int instance, const int n_variables) :
            BBOB(13, instance, n_variables, "SharpRidge")
        {
        }
    };
}
