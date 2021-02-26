#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{
    class GriewankRosenBrock final : public BBOB<GriewankRosenBrock>
    {
    protected:
        std::vector<double> evaluate(std::vector<double> &x) override
        {
            auto result = 0.0;
            for (auto i = 0; i < meta_data_.n_variables - 1; ++i) {
                const auto c1 = 100.0 * pow(pow(x.at(i), 2.0) - x.at(i + 1), 2.0);
                const auto c2 = pow(1.0 - x.at(i), 2.0);
                const auto z =  c1 + c2;
                result += z / 4000. - cos(z);
            }
            return { 10. + 10. * result / static_cast<double>(meta_data_.n_variables - 1) };
        }
    
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            transform_vars_affine_evaluate_function(x, transformation_state_.transformation_matrix,
                                                    transformation_state_.transformation_base);
            transform_vars_shift_evaluate_function(x, meta_data_.objective.x);
            return x;
        }
    
    public:
        GriewankRosenBrock(const int instance, const int n_variables) :
            BBOB(19, instance, n_variables, "GriewankRosenBrock")
        {
            for (auto &e : meta_data_.objective.x)
                e = -0.5;

            //TODO how to correctly set xopt
    
            const auto scale = std::max(1., sqrt(n_variables) / 8.);

            for (auto i = 0; i < n_variables; ++i)
                for (auto j = 0; j < n_variables; ++j)
                    transformation_state_.second_rotation[i][j] *= scale;
    
            transformation::coco::bbob2009_copy_rotation_matrix(
                transformation_state_.second_rotation, transformation_state_.transformation_matrix,
                transformation_state_.transformation_base, n_variables);
        }
    };
}