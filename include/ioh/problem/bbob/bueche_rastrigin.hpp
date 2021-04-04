#pragma once

#include "rastrigin.hpp"

namespace ioh::problem::bbob
{
    class BuecheRastrigin final: public RastriginBase<BuecheRastrigin>
    {
        const double penalty_factor_ = 100.0;

    protected:
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;            
            transform_vars_shift_evaluate_function(x, objective_.x);
            transform_vars_oscillate_evaluate_function(x);
            transform_vars_brs_evaluate(x);
            return x;
        }

        std::vector<double> transform_objectives(std::vector<double> y) override
        {
            using namespace transformation::coco;
            transform_obj_shift_evaluate_function(y, objective_.y.at(0));
            transform_obj_penalize_evaluate(state_.current.x, constraint_.lb.at(0),
                                            constraint_.ub.at(0), penalty_factor_, y);
            return y;
        }
    public:
        BuecheRastrigin(const int instance, const int n_variables) :
            RastriginBase(4,  instance, n_variables, "BuecheRastrigin")
        {
            for (auto &e : objective_.x)
                e = fabs(e);
        }
    };
}
