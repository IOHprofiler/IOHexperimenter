#pragma once

#include <algorithm>
#include <iostream>
#include <limits>
#include <math.h>
#include <random>
#include <vector>

#include "ioh/problem/single.hpp"
#include "ioh/problem/transformation.hpp"

namespace ioh::problem
{
    class DynamicBinVal : public IntegerSingleObjective
    {
    public:

        DynamicBinVal(
                const int problem_id,
                const int instance,
                const int n_variables,
                const std::string &name
            ) :
            IntegerSingleObjective(
                MetaData(
                    problem_id,
                    instance,
                    name,
                    n_variables,
                    common::OptimizationType::MAX
                ),
                Bounds<int>(n_variables, 0, 1)
            ) { }

        virtual int step() = 0;
    };

    template <typename ProblemType>
    struct DynamicBinValProblem :
        DynamicBinVal,
        AutomaticProblemRegistration<ProblemType, DynamicBinVal>,
        AutomaticProblemRegistration<ProblemType, IntegerSingleObjective>
    {
        using DynamicBinVal::DynamicBinVal;
    };
}