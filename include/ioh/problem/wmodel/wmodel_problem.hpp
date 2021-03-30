#pragma once

#include "ioh/problem/problem.hpp"

namespace ioh::problem
{
    struct WModel: Integer
    {
        WModel(const int instance, const int n_variables, const std::string& name) :
            Integer(MetaData(1, instance, name, n_variables, 1, common::OptimizationType::Maximization),
                Constraint<int>(n_variables, 0, 1)
                )
        {
            objective_.x = std::vector<int>(n_variables);
        }
    };


    template <typename ProblemType>
    struct WModelProblem : WModel,
        AutomaticProblemRegistration<ProblemType, WModel>,
        AutomaticProblemRegistration<ProblemType, Integer>
    {
        using WModel::WModel;
    };
}

