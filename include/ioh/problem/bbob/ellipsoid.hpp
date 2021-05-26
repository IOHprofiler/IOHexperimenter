#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    template <typename T>
    class EllipsoidBase : public BBOProblem<T>
    {
    protected:
        double evaluate(const std::vector<double> &x) override
        {
            auto result = x.at(0) * x.at(0);
            for (auto i = 1; i < this->meta_data_.n_variables; ++i)
                result += this->transformation_state_.conditions.at(i) * x.at(i) * x.at(i);
            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, this->objective_.x);
            oscillate(x);
            return x;
        }

    public:
        EllipsoidBase(const int problem_id, const int instance, const int n_variables, const std::string &name) :
            BBOProblem<T>(problem_id, instance, n_variables, name)
           
        {
            static const auto condition = 1.0e6;
            for (auto i = 1; i < this->meta_data_.n_variables; ++i)
                this->transformation_state_.conditions[i] = pow(condition, this->transformation_state_.exponents.at(i));
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
