#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    class Katsuura final : public BBOProblem<Katsuura>

    {
        double exponent_;
        double factor_;

    protected:
        std::vector<double> evaluate(std::vector<double> &x) override
        {
            std::vector<double> result = {1.0};
            for (auto i = 0; i < meta_data_.n_variables; ++i)
            {
                double z = 0;
                for (size_t j = 1; j < 33; ++j)
                    z += fabs(transformation_state_.exponents.at(j) * x.at(i)
                            - floor(transformation_state_.exponents.at(j) * x.at(i) + 0.5))
                        / transformation_state_.exponents.at(j);

                result[0] *= pow(1.0 + (static_cast<double>(i) + 1) * z, exponent_);
            }
            result[0] = factor_ * (-1. + result.at(0));
            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            transform_vars_shift_evaluate_function(x, objective_.x);
            transform_vars_affine_evaluate_function(
                x, transformation_state_.second_transformation_matrix,
                transformation_state_.transformation_base);
            return x;
        }

        std::vector<double> transform_objectives(std::vector<double> y) override
        {
            using namespace transformation::coco;
            static const auto penalty_factor = 1.0;
            transform_obj_shift_evaluate_function(y, objective_.y.at(0));
            transform_obj_penalize_evaluate(state_.current.x, constraint_.lb.at(0),
                                            constraint_.ub.at(0), penalty_factor, y);
            return y;
        }

    public:
        Katsuura(const int instance, const int n_variables) :
            BBOProblem(23, instance, n_variables, "Katsuura", sqrt(100.0)),
            exponent_(10. / pow(static_cast<double>(meta_data_.n_variables), 1.2)),
            factor_(10. / static_cast<double>(meta_data_.n_variables) / static_cast<double>(meta_data_.n_variables))
        {
            transformation_state_.exponents.resize(33);
            for (auto i = 1; i < 33; ++i)
                transformation_state_.exponents[i] = pow(2., static_cast<double>(i));
        }
    };
}
