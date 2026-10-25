// Built-in entries of the PBO factory
#include "families.hpp"

namespace ioh::common
{
    void add_builtins(Factory<problem::PBO, int, int> &factory) { registry::add_pbo(factory); }
} // namespace ioh::common
