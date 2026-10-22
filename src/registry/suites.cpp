// Built-in entries of the suite factories. They get the ids 1-4 in this order; suite ids are not visible
// from Python, and before the registry was compiled they depended on the static initialization order.
#include "families.hpp"

namespace ioh::common
{
    void add_builtins(Factory<suite::Suite<problem::RealSingleObjective>, std::vector<int>, std::vector<int>,
                              std::vector<int>> &factory)
    {
        using Registry = suite::SuiteRegistry<problem::RealSingleObjective>;
        Registry::include<suite::Real>(factory);
        Registry::include<suite::BBOB>(factory);
        Registry::include<suite::SBOX>(factory);
        Registry::include<suite::RealStarDiscrepancy>(factory);
    }

    void add_builtins(Factory<suite::Suite<problem::IntegerSingleObjective>, std::vector<int>, std::vector<int>,
                              std::vector<int>> &factory)
    {
        using Registry = suite::SuiteRegistry<problem::IntegerSingleObjective>;
        Registry::include<suite::Integer>(factory);
        Registry::include<suite::PBO>(factory);
        Registry::include<suite::Submodular>(factory);
        Registry::include<suite::IntegerStarDiscrepancy>(factory);
    }
} // namespace ioh::common
