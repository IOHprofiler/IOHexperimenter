#pragma once

#include "rastrigin.hpp"

namespace ioh::problem::bbob
{
    class BuecheRastrigin final: public RastriginBase<BuecheRastrigin>
    {
        const double penalty_factor_ = 100.0;
        std::vector<double> raw_x_;
    protected:
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            raw_x_ = x;
            transform_vars_shift_evaluate_function(x, meta_data_.objective.x);
            transform_vars_oscillate_evaluate_function(x);
            transform_vars_brs_evaluate(x);
            return x;
        }

        std::vector<double> transform_objectives(std::vector<double> y) override
        {
            using namespace transformation::coco;
            transform_obj_shift_evaluate_function(y, meta_data_.objective.y.at(0));
            transform_obj_penalize_evaluate(raw_x_, constraint_.lb.at(0),
                                            constraint_.ub.at(0), penalty_factor_, y);
            return y;
        }
    public:
        BuecheRastrigin(const int instance, const int n_variables) :
            RastriginBase(4,  instance, n_variables, "BuecheRastrigin"),
            raw_x_(n_variables)
        {
            for (auto &e : meta_data_.objective.x)
                e = fabs(e);
        }
    };
}
