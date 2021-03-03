#pragma once

#include "ioh/problem/problem.hpp"

namespace ioh::problem
{
    class PBO : public Integer
    {
    protected:
        std::vector<int> transform_variables(std::vector<int> x) override
        {
            if (meta_data_.instance > 1 && meta_data_.instance <= 50)
                transformation::methods::transform_vars_xor(x, meta_data_.instance);
            else if (meta_data_.instance > 50 && meta_data_.instance <= 100)
                transformation::methods::transform_vars_sigma(x, meta_data_.instance);
            return x;
        }

        std::vector<double> transform_objectives(std::vector<double> y) override
        {
            if (meta_data_.instance > 1)
            {
                transformation::methods::transform_obj_scale(y, meta_data_.instance);
                transformation::methods::transform_obj_shift(y, meta_data_.instance);
            }
            return y;
        }

    public:
        PBO(const int problem_id, const int instance, const int n_variables, const std::string &name) :
            Integer(MetaData(problem_id, instance, name, n_variables, 1,
                                    common::OptimizationType::maximization))
        {
        } 
    };

    template <typename ProblemType>
    class PBOProblem : public PBO,
               AutomaticProblemRegistration<ProblemType, PBO>,
               AutomaticProblemRegistration<ProblemType, Integer>
    {
    public:
        using PBO::PBO;
    };
}
