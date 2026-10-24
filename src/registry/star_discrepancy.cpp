// Built-in entries of the real and integer star discrepancy factories
#include "families.hpp"

namespace ioh::common
{
    void add_builtins(Factory<problem::star_discrepancy::real::StarDiscrepancy, int, int> &factory)
    {
        registry::add_star_discrepancy_real(factory);
    }

    void add_builtins(Factory<problem::star_discrepancy::integer::StarDiscrepancy, int, int> &factory)
    {
        registry::add_star_discrepancy_integer(factory);
    }
} // namespace ioh::common
