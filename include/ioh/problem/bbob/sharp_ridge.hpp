#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    class SharpRidge final : public BBOProblem<SharpRidge>

    {
        int n_linear_dimensions_;
    protected:
        std::vector<double> evaluate(std::vector<double> &x) override
        {
            static const auto alpha = 100.0;

            std::vector<double> result{0.0};
            for (auto i = n_linear_dimensions_; i < meta_data_.n_variables; ++i)
                result[0] += x.at(i) * x.at(i);

            result[0] = alpha * sqrt(result.at(0) / n_linear_dimensions_);
            for (auto i = 0; i < n_linear_dimensions_; ++i)
                result[0] += x.at(i) * x.at(i) / n_linear_dimensions_;

            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            transform_vars_shift_evaluate_function(x, meta_data_.objective.x);
            transform_vars_affine_evaluate_function(x, transformation_state_.second_transformation_matrix, transformation_state_.transformation_base);
            return x;
        }

    public:
        SharpRidge(const int instance, const int n_variables) :
            BBOProblem(13, instance, n_variables, "SharpRidge"),
        n_linear_dimensions_(static_cast<int>(
            ceil(meta_data_.n_variables <= 40 ? 1 : meta_data_.n_variables / 40.0)))
        {
        }
    };
}
