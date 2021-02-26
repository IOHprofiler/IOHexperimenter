#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{
    template <typename T>
    class EllipsoidBase : public BBOB<T>
    {
    protected:
        std::vector<double> evaluate(std::vector<double> &x) override
        {
            auto result = x.at(0) * x.at(0);
            for (size_t i = 1; i < meta_data_.n_variables; ++i)
                result += transformation_state_.conditions.at(i) * x.at(i) * x.at(i);
            return {result};
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            transform_vars_shift_evaluate_function(x, meta_data_.objective.x);
            transform_vars_oscillate_evaluate_function(x);
            return x;
        }

    public:
        EllipsoidBase(const int problem_id, const int instance, const int n_variables, const std::string &name) :
            BBOB(problem_id, instance, n_variables, name)
        {
            static const auto condition = 1.0e6;
            for (auto i = 1; i < meta_data_.n_variables; ++i)
                transformation_state_.conditions[i] = pow(condition, transformation_state_.exponents.at(i));
        }
    };

    class Ellipsoid final : public EllipsoidBase<Ellipsoid>
    {
    public:
        Ellipsoid(const int instance, const int n_variables) :
            EllipsoidBase(2, instance, n_variables, "Ellipsoid")
        {
        }
    };
}
