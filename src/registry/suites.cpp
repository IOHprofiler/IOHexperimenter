// Built-in entries of the suite factories, with the ids and names the suite classes declare (registry_id,
// registry_name): Real 1, BBOB 2, SBOX 3, RealStarDiscrepancy 4; Integer 1, PBO 2, Submodular 3,
// IntegerStarDiscrepancy 4
#include "families.hpp"

namespace ioh::common
{
    void add_builtins(Factory<suite::Suite<problem::RealSingleObjective>, std::vector<int>, std::vector<int>,
                              std::vector<int>> &factory)
    {
        registry::add_suites<problem::RealSingleObjective, suite::Real, suite::BBOB, suite::SBOX,
                             suite::RealStarDiscrepancy>(factory);
    }

    void add_builtins(Factory<suite::Suite<problem::IntegerSingleObjective>, std::vector<int>, std::vector<int>,
                              std::vector<int>> &factory)
    {
        registry::add_suites<problem::IntegerSingleObjective, suite::Integer, suite::PBO, suite::Submodular,
                             suite::IntegerStarDiscrepancy>(factory);
    }
} // namespace ioh::common
