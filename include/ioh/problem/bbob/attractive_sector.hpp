#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{
    class AttractiveSector final : public BBOB<AttractiveSector>
    {
    protected:
        std::vector<double> evaluate(std::vector<double> &x) override
        {
            std::vector<double> result{ 0.0 };
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                result[0] += x.at(i) * x.at(i) * (1. + 9999.0 * (meta_data_.objective.x.at(i) * x.at(i) > 0.0));
            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            transform_vars_shift_evaluate_function(x, meta_data_.objective.x);
            transform_vars_affine_evaluate_function(x, transformation_state_.m, transformation_state_.b);
            return x;
        }

        std::vector<double> transform_objectives(std::vector<double> y) override
        {
            using namespace transformation::coco;
            transform_obj_oscillate_evaluate(y);
            transform_obj_power_evaluate(y, 0.9);
            transform_obj_shift_evaluate_function(y, meta_data_.objective.y.at(0));
            return y;
        }

    public:
        AttractiveSector(const int instance, const int n_variables) :
            BBOB(6, instance, n_variables, "AttractiveSector")
        {
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                for (auto j = 0; j < meta_data_.n_variables; ++j)
                    for (auto k = 0; k < meta_data_.n_variables; ++k)
                        transformation_state_.m[i][j] += transformation_state_.rot1.at(i).at(k)
                            * pow(sqrt(10.0), transformation_state_.exponents.at(k))
                            * transformation_state_.rot2.at(k).at(j);
        }
    };
}
