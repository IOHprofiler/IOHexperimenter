#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    class AttractiveSector final : public BBOProblem<AttractiveSector>
    {
    protected:
        double evaluate(const std::vector<double> &x) override
        {
            auto result =  0.0 ;
            for (auto i = 0; i < meta_data_.n_variables; ++i)
                result += x.at(i) * x.at(i) * (1. + 9999.0 * (objective_.x.at(i) * x.at(i) > 0.0));
            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, objective_.x);
            affine(x, transformation_state_.second_transformation_matrix, transformation_state_.transformation_base);
            return x;
        }

        double transform_objectives(const double y) override
        {
            using namespace transformation::objective;
            return shift(pow(oscillate(y), .9), objective_.y);
        }

    public:
        AttractiveSector(const int instance, const int n_variables) :
            BBOProblem(6, instance, n_variables, "AttractiveSector")
        {
        }
    };
}
