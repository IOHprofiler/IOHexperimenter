// Built-in entries of the factories of all real and all integer single-objective problems
#include "families.hpp"

namespace ioh::common
{
    void add_builtins(Factory<problem::RealSingleObjective, int, int> &factory)
    {
        registry::add_bbob<problem::BBOB>(factory);
        registry::add_cec2013(factory);
        registry::add_cec2022(factory);
        registry::add_star_discrepancy_real(factory);
    }

    void add_builtins(Factory<problem::IntegerSingleObjective, int, int> &factory)
    {
        registry::add_pbo(factory);
        registry::add_graph(factory);
        registry::add_star_discrepancy_integer(factory);
        registry::add_dynamic_bin_val(factory);
    }
} // namespace ioh::common
